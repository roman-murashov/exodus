#ifndef __VIEWMODELBASE_H__
#define __VIEWMODELBASE_H__
#include "IViewModel.h"
#include "IView.h"

class ViewModelBase :public IViewModel
{
public:
	//Constructors
	ViewModelBase(const std::wstring& amenuHandlerName, int aviewModelID, bool aownerIsSystem, bool aownerIsDevice, const std::wstring& adeviceInstanceName, unsigned int amoduleID);

	//View creation and deletion
	virtual IView* CreateView() = 0;
	virtual void DeleteView(IView* aview) = 0;

	//View management functions
	virtual bool OpenView(IViewModelNotifier* anotifier, void* aparentWindow, int xpos, int ypos);
	virtual void CloseView();
	virtual void ShowView();
	virtual void HideView();
	virtual void ActivateView();
	virtual void NotifyViewClosed(IView* aview);

	//View owner functions
	virtual bool IsViewOwnerDevice() const;
	virtual bool IsViewOwnerSystem() const;
	virtual void SetViewOwnerAsSystem();
	virtual unsigned int GetViewOwnerModuleID() const;

	//Position and size functions
	virtual bool CanResizeView() const;
	virtual void GetViewPosition(int& xpos, int& ypos) const;
	virtual void GetViewSize(int& sizex, int& sizey) const;
	virtual void SetViewPosition(int xpos, int ypos);
	virtual void SetViewSize(int sizex, int sizey);

	//Notifiers
	virtual void NotifyViewOpened();
	virtual void NotifyViewClosed();

	//State functions
	virtual int GetViewID() const;
	virtual bool SaveViewLayoutState(IHeirarchicalStorageNode& node) const;
	virtual bool LoadViewLayoutState(IHeirarchicalStorageNode& node);
	virtual bool DoesWindowHandleMatchView(void* awindowHandle) const;

protected:
	//View owner functions
	virtual void SetViewOwnerAsDeviceInternal(const wchar_t* deviceInstanceName, unsigned int moduleID);
	virtual const wchar_t* GetViewOwnerDeviceInstanceNameInternal() const;

	//Title functions
	virtual const wchar_t* GetViewTitleInternal() const;

	//State functions
	virtual const wchar_t* GetMenuHandlerNameInternal() const;

	//View management functions
	IView* GetOpenView() const;

private:
	bool viewOpen;
	IViewModelNotifier* notifier;
	IView* view;
	int viewModelID;
	bool viewOwnerIsDevice;
	bool viewOwnerIsSystem;
	std::wstring viewOwnerDeviceInstanceName;
	unsigned int viewOwnerModuleID;
	std::wstring menuHandlerName;

	mutable std::wstring viewTitleCache;
};

#endif
