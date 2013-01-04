#include "WindowFunctions/WindowFunctions.pkg"
#include "Debug/Debug.pkg"
#include "ExodusInterface.h"
#include "resource.h"

//##TODO## Fix the static module linking process. We're not actively using it anymore, but
//we want to retain support for it. Some users may have cases where it is useful to be
//able to compile versions of our emulation platform which are pre-assembled to emulate a
//particular system.

//If we're statically linking to any devices, include the interface functions for those
//devices here.
#ifndef EX_DLLINTERFACE
#include "../Devices/315-5313/interface.h"
#include "../Devices/M68000/interface.h"
#include "../Devices/MD1600IO/interface.h"
#include "../Devices/Memory/interface.h"
#include "../Devices/Z80/interface.h"
#include "../Devices/YM2612/interface.h"
#include "../Devices/SN76489/interface.h"
#endif

//----------------------------------------------------------------------------------------
//WinMain function
//----------------------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	//Register our minidump exception handler
	//##TODO## Consider putting a build number into the filename
	//##TODO## Consider making a text crash report file too, which reports on memory
	//usage, build version, platform info, and other such details.
	RegisterMinidumpExceptionHandler(L"Exodus", L"Crash Reports", false);

	//Create a debug command console
	AllocConsole();
	BindStdHandlesToConsole();

	//Construct the system object
	System systemObject((void*)GetModuleHandle(NULL));

	//If we're statically linking to any devices, bind them to the system here.
#ifndef EX_DLLINTERFACE
	systemObject.RegisterDevice(GetA04000Library(), (Device::AssemblyHandle)GetModuleHandle(NULL));
	systemObject.RegisterDevice(GetA06000Library(), (Device::AssemblyHandle)GetModuleHandle(NULL));
	systemObject.RegisterDevice(GetA11100Library(), (Device::AssemblyHandle)GetModuleHandle(NULL));
	systemObject.RegisterDevice(GetA10000Library(), (Device::AssemblyHandle)GetModuleHandle(NULL));
	systemObject.RegisterDevice(GetM68000Library(), (Device::AssemblyHandle)GetModuleHandle(NULL));
	systemObject.RegisterDevice(GetRAMLibrary(), (Device::AssemblyHandle)GetModuleHandle(NULL));
	systemObject.RegisterDevice(GetSharedRAMLibrary(), (Device::AssemblyHandle)GetModuleHandle(NULL));
	systemObject.RegisterDevice(GetTimedRAMLibrary(), (Device::AssemblyHandle)GetModuleHandle(NULL));
	systemObject.RegisterDevice(GetROMLibrary(), (Device::AssemblyHandle)GetModuleHandle(NULL));
	systemObject.RegisterDevice(GetSN76489Library(), (Device::AssemblyHandle)GetModuleHandle(NULL));
	systemObject.RegisterDevice(GetVDPLibrary(), (Device::AssemblyHandle)GetModuleHandle(NULL));
	systemObject.RegisterDevice(GetYM2612Library(), (Device::AssemblyHandle)GetModuleHandle(NULL));
	systemObject.RegisterDevice(GetZ80Library(), (Device::AssemblyHandle)GetModuleHandle(NULL));
#endif

	//Create the main interface window
	ExodusInterface exodusInterface(systemObject);
	HWND hwnd = exodusInterface.CreateMainInterface(hInstance);
	if(hwnd == NULL)
	{
		return 1;
	}

	//Initialize the system, and load all modules and settings.
	//##TODO## Separate this out. This should be a message or the like which is posted to
	//the message queue for the main window, and is then picked up by the UI thread and
	//executed. It should display progress for module loading.
	if(!exodusInterface.InitializeSystem())
	{
		return 2;
	}

	//Load the keyboard accelerator table
	HACCEL acceleratorTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATORS));
	if(acceleratorTable == NULL)
	{
		return 3;
	}

	//Begin the message loop
	MSG msg;
	bool done = false;
	while(!done)
	{
		BOOL getMessageReturn = GetMessage(&msg, NULL, 0, 0);
		if(getMessageReturn == 0)
		{
			done = true;
		}
		else if(getMessageReturn != -1)
		{
			if(TranslateAccelerator(hwnd, acceleratorTable, &msg) == 0)
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}

	//Destroy the keyboard accelerator table
	DestroyAcceleratorTable(acceleratorTable);

	return 0;
}
