#include "S315_5313.h"
#ifndef __S315_5313_DEBUGSETTINGSVIEWMODEL_H__
#define __S315_5313_DEBUGSETTINGSVIEWMODEL_H__
#include "SystemInterface/SystemInterface.pkg"

class S315_5313::DebugSettingsViewModel :public ViewModelBase
{
public:
	//Constructors
	DebugSettingsViewModel(const std::wstring& amenuHandlerName, int aviewModelID, S315_5313* adevice);

	//View creation and deletion
	virtual IView* CreateView();
	virtual void DeleteView(IView* aview);

private:
	S315_5313* device;
};

#endif
