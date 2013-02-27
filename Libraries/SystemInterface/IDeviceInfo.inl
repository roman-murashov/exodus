//----------------------------------------------------------------------------------------
//Setters
//----------------------------------------------------------------------------------------
void IDeviceInfo::SetDeviceSettings(const std::wstring& adeviceName, IDevice* (aAllocator)(const wchar_t*, unsigned int), void (aDestructor)(IDevice*), unsigned int adeviceVersionNo)
{
	SetDeviceName(adeviceName);
	SetDeviceVersionNo(adeviceVersionNo);
	SetDeviceAllocators(aAllocator, aDestructor);
}

//----------------------------------------------------------------------------------------
void IDeviceInfo::SetDeviceName(const std::wstring& adeviceName)
{
	SetDeviceNameInternal(adeviceName.c_str());
}
