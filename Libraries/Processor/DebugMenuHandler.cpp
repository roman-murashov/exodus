#include "DebugMenuHandler.h"
#include "ControlViewModel.h"
#include "BreakpointViewModel.h"
#include "WatchpointViewModel.h"
#include "CallStackViewModel.h"
#include "TraceViewModel.h"
#include "DisassemblyOldViewModel.h"
#include "DisassemblyViewModel.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
Processor::DebugMenuHandler::DebugMenuHandler(Processor* adevice)
:MenuHandlerBase(L"ProcessorDebugMenu"), device(adevice)
{}

//----------------------------------------------------------------------------------------
//Management functions
//----------------------------------------------------------------------------------------
void Processor::DebugMenuHandler::GetMenuItems(std::list<MenuItemDefinition>& menuItems) const
{
	menuItems.push_back(MenuItemDefinition(MENUITEM_CONTROL, L"ProcessorControl", L"Processor Control", true));
	menuItems.push_back(MenuItemDefinition(MENUITEM_BREAKPOINTS, L"Breakpoints", L"Breakpoints", true));
	menuItems.push_back(MenuItemDefinition(MENUITEM_WATCHPOINTS, L"Watchpoints", L"Watchpoints", true));
	menuItems.push_back(MenuItemDefinition(MENUITEM_CALLSTACK, L"CallStack", L"Call Stack", true));
	menuItems.push_back(MenuItemDefinition(MENUITEM_TRACE, L"Trace", L"Trace", true));
	menuItems.push_back(MenuItemDefinition(MENUITEM_DISASSEMBLY, L"Disassembly", L"Disassembly", true));
	menuItems.push_back(MenuItemDefinition(MENUITEM_DISASSEMBLYOLD, L"OldDisassembly", L"Old Disassembly", true));
}

//----------------------------------------------------------------------------------------
IViewModel* Processor::DebugMenuHandler::CreateViewModelForItem(int menuItemID, const std::wstring& viewModelName)
{
	switch(menuItemID)
	{
	case MENUITEM_CONTROL:
		return new ControlViewModel(GetMenuHandlerName(), viewModelName, MENUITEM_CONTROL, device);
	case MENUITEM_BREAKPOINTS:
		return new BreakpointViewModel(GetMenuHandlerName(), viewModelName, MENUITEM_BREAKPOINTS, device);
	case MENUITEM_WATCHPOINTS:
		return new WatchpointViewModel(GetMenuHandlerName(), viewModelName, MENUITEM_WATCHPOINTS, device);
	case MENUITEM_CALLSTACK:
		return new CallStackViewModel(GetMenuHandlerName(), viewModelName, MENUITEM_CALLSTACK, device);
	case MENUITEM_TRACE:
		return new TraceViewModel(GetMenuHandlerName(), viewModelName, MENUITEM_TRACE, device);
	case MENUITEM_DISASSEMBLY:
		return new DisassemblyViewModel(GetMenuHandlerName(), viewModelName, MENUITEM_DISASSEMBLY, device);
	case MENUITEM_DISASSEMBLYOLD:
		return new DisassemblyOldViewModel(GetMenuHandlerName(), viewModelName, MENUITEM_DISASSEMBLYOLD, device);
	}
	return 0;
}

//----------------------------------------------------------------------------------------
void Processor::DebugMenuHandler::DeleteViewModelForItem(int menuItemID, IViewModel* viewModel)
{
	delete viewModel;
}
