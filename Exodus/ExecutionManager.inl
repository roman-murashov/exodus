//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
ExecutionManager::ExecutionManager()
:totalDeviceCount(0), deviceCount(0), suspendDeviceCount(0), transientDeviceCount(0)
{}

//----------------------------------------------------------------------------------------
//Device functions
//----------------------------------------------------------------------------------------
void ExecutionManager::AddDevice(DeviceContext* device)
{
	//Add the specified device to the device arrays
	deviceArray.push_back(device);
	if(device->UsesExecuteSuspend())
	{
		suspendDeviceArray.push_back(device);
	}
	if(device->UsesTransientExecution())
	{
		transientDeviceArray.push_back(device);
	}

	//Update the device counts
	deviceCount = deviceArray.size();
	suspendDeviceCount = suspendDeviceArray.size();
	transientDeviceCount = transientDeviceArray.size();
	totalDeviceCount = (ReferenceCounterType)deviceCount;

	//Resize the message results array
	command.timesliceResult.resize(deviceCount);
	command.contextResult.resize(deviceCount);
}

//----------------------------------------------------------------------------------------
void ExecutionManager::RemoveDevice(DeviceContext* device)
{
	bool done;
	std::vector<DeviceContext*>::iterator i;

	//Remove the specified device from the device array
	done = false;
	i = deviceArray.begin();
	while(!done && (i != deviceArray.end()))
	{
		if(*i == device)
		{
			deviceArray.erase(i);
			done = true;
		}
		else
		{
			++i;
		}
	}

	//Remove the specified device from the suspend device array
	done = false;
	i = suspendDeviceArray.begin();
	while(!done && (i != suspendDeviceArray.end()))
	{
		if(*i == device)
		{
			suspendDeviceArray.erase(i);
			done = true;
		}
		else
		{
			++i;
		}
	}

	//Remove the specified device from the transient device array
	done = false;
	i = transientDeviceArray.begin();
	while(!done && (i != transientDeviceArray.end()))
	{
		if(*i == device)
		{
			transientDeviceArray.erase(i);
			done = true;
		}
		else
		{
			++i;
		}
	}

	//Update the device counts
	deviceCount = deviceArray.size();
	suspendDeviceCount = suspendDeviceArray.size();
	transientDeviceCount = transientDeviceArray.size();
	totalDeviceCount = (ReferenceCounterType)deviceCount;

	//Resize the message results array
	command.timesliceResult.resize(deviceCount);
	command.contextResult.resize(deviceCount);
}

//----------------------------------------------------------------------------------------
void ExecutionManager::ClearAllDevices()
{
	//Clear all device arrays
	deviceArray.clear();
	suspendDeviceArray.clear();
	transientDeviceArray.clear();
	command.timesliceResult.clear();
	command.contextResult.clear();

	//Initialize the device counts
	deviceCount = 0;
	suspendDeviceCount = 0;
	totalDeviceCount = 0;
}

//----------------------------------------------------------------------------------------
//Execute functions
//----------------------------------------------------------------------------------------
void ExecutionManager::NotifyUpcomingTimeslice(double nanoseconds)
{
	boost::mutex::scoped_lock lock(commandMutex);
	command.type = DeviceContext::DeviceContextCommand::TYPE_NOTIFYUPCOMINGTIMESLICE;
	command.timeslice = nanoseconds;
	pendingDeviceCount = totalDeviceCount;
	commandSent.notify_all();
	commandProcessed.wait(lock);
}

//----------------------------------------------------------------------------------------
void ExecutionManager::NotifyBeforeExecuteCalled()
{
	boost::mutex::scoped_lock lock(commandMutex);
	command.type = DeviceContext::DeviceContextCommand::TYPE_NOTIFYBEFOREEXECUTECALLED;
	pendingDeviceCount = totalDeviceCount;
	commandSent.notify_all();
	commandProcessed.wait(lock);
}

//----------------------------------------------------------------------------------------
void ExecutionManager::NotifyAfterExecuteCalled()
{
	boost::mutex::scoped_lock lock(commandMutex);
	command.type = DeviceContext::DeviceContextCommand::TYPE_NOTIFYAFTEREXECUTECALLED;
	pendingDeviceCount = totalDeviceCount;
	commandSent.notify_all();
	commandProcessed.wait(lock);
}

//----------------------------------------------------------------------------------------
void ExecutionManager::ExecuteTimeslice(double nanoseconds)
{
	boost::mutex::scoped_lock lock(commandMutex);

	//Enable execution suspend features for devices that support it
	EnableTimesliceExecutionSuspend();

	//Start all devices executing the new timeslice
	command.type = DeviceContext::DeviceContextCommand::TYPE_EXECUTETIMESLICE;
	command.timeslice = nanoseconds;
	suspendedThreadCount = 0;
	pendingDeviceCount = totalDeviceCount;
	commandSent.notify_all();
	commandProcessed.wait(lock);

	//Wait for all devices to finish executing the timeslice
	command.type = DeviceContext::DeviceContextCommand::TYPE_WAITFOREXECUTECOMPLETE;
	pendingDeviceCount = totalDeviceCount;
	commandSent.notify_all();
	commandProcessed.wait(lock);

	//Disable execution suspend features for devices that support it. Note that execution
	//suspend may be disabled automatically before the timeslice is completed if all
	//remaining devices are in the suspend state.
	DisableTimesliceExecutionSuspend();
}

//----------------------------------------------------------------------------------------
void ExecutionManager::Commit()
{
	boost::mutex::scoped_lock lock(commandMutex);
	command.type = DeviceContext::DeviceContextCommand::TYPE_COMMIT;
	pendingDeviceCount = totalDeviceCount;
	commandSent.notify_all();
	commandProcessed.wait(lock);
}

//----------------------------------------------------------------------------------------
void ExecutionManager::Rollback()
{
	boost::mutex::scoped_lock lock(commandMutex);
	command.type = DeviceContext::DeviceContextCommand::TYPE_ROLLBACK;
	pendingDeviceCount = totalDeviceCount;
	commandSent.notify_all();
	commandProcessed.wait(lock);
}

//----------------------------------------------------------------------------------------
void ExecutionManager::Initialize()
{
	//Negate the current output line state for all devices we are about to initialize. We
	//need to do this, as the Initialize routine may alter the internal state of the
	//device in such a way that line state changes which have been conditionally asserted
	//for a future point in time should no longer be triggered. The Initialize routine is
	//not allowed to interact with external devices however, so it cannot revoke any
	//pending line state changes that have already been asserted. By explicitly negating
	//all output line state here, we ensure that there are no pending line state changes
	//asserted before we initialize the devices.
	for(size_t i = 0; i < deviceCount; ++i)
	{
		deviceArray[i]->GetTargetDevice()->NegateCurrentOutputLineState();
	}

	//Initialize the devices. Note that we only perform initialization on a device when
	//execution for that device is suspended. This is critical, in order to ensure that
	//the device does not have any actively running threads, such as render threads, which
	//might be working with the internal state data we are about to initialize. As the
	//device is currently suspended, we can't send this command in parallel to each
	//device, as the command worker thread for each device is also suspended, so we
	//initialize each device serially on the one thread here.
	for(size_t i = 0; i < deviceCount; ++i)
	{
		deviceArray[i]->Initialize();
	}

	//Re-assert the current output line state for all devices we just initialized. This is
	//required, as the initialization routine for a device may change its internal state
	//in a way that would affect its asserted output line state, but the Initialize method
	//is not allowed to interact with external devices. A call to the
	//AssertCurrentOutputLineState method ensures that the correct external line state can
	//now be asserted for all initialized devices.
	for(size_t i = 0; i < deviceCount; ++i)
	{
		deviceArray[i]->GetTargetDevice()->AssertCurrentOutputLineState();
	}
}

//----------------------------------------------------------------------------------------
//Timing functions
//----------------------------------------------------------------------------------------
double ExecutionManager::GetNextTimingPoint(double maximumTimeslice, DeviceContext*& nextDeviceStep, unsigned int& nextDeviceStepContext)
{
	boost::mutex::scoped_lock lock(commandMutex);
	command.type = DeviceContext::DeviceContextCommand::TYPE_GETNEXTTIMINGPOINT;
	pendingDeviceCount = totalDeviceCount;
	commandSent.notify_all();
	commandProcessed.wait(lock);

	//Determine the maximum length of time all devices can run unsynchronized before the
	//next timing point
	double timeslice = maximumTimeslice;
	nextDeviceStep = 0;
	for(size_t i = 0; i < deviceCount; ++i)
	{
		double deviceTimingPoint = command.timesliceResult[i];
		if((deviceTimingPoint < timeslice) && (deviceTimingPoint >= 0))
		{
			timeslice = deviceTimingPoint;
			nextDeviceStep = deviceArray[i];
			nextDeviceStepContext = command.contextResult[i];
		}
	}

	return timeslice;
}

//----------------------------------------------------------------------------------------
//Worker thread control
//----------------------------------------------------------------------------------------
void ExecutionManager::BeginExecution()
{
	pendingDeviceCount = totalDeviceCount;
	for(size_t i = 0; i < deviceCount; ++i)
	{
		deviceArray[i]->BeginExecution(i, pendingDeviceCount, suspendedThreadCount, commandMutex, commandSent, commandProcessed, this, command);
	}
}

//----------------------------------------------------------------------------------------
void ExecutionManager::SuspendExecution()
{
	boost::mutex::scoped_lock lock(commandMutex);
	command.type = DeviceContext::DeviceContextCommand::TYPE_SUSPENDEXECUTION;
	pendingDeviceCount = totalDeviceCount;
	commandSent.notify_all();
	commandProcessed.wait(lock);
}
