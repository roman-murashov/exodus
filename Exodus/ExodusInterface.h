/*--------------------------------------------------------------------------------------*\
Things to do:
-Add a window management menu, where the list of loaded views can be enumerated and
managed.
-Add an auto-layout option for window control, which will use autopositioning to layout
all the existing views.
-Save the workspace client window size and maximize state in the workspace save file
-Add a "Size main window to fit dialogs" option, which will find the extents of the loaded
views, and fit the client frame around it.
-Add snapping when dragging windows, so that the windows will snap to corners and edges of
other loaded windows.
-Consider adding advanced logic when dragging client windows, so that when dragging a
window, it "moves" other windows out of the way, to allow it to be inserted between
windows. The user would see the windows move out of the way as the drag operation is being
performed, but the windows would return to their original position if the user continues
to move the window being dragged. Windows should "push" other windows as the window is
being dragged, so that if a window is being moved out of the way to the right, and there's
some space next to it, then another window to the right, the window being moved should
swallow up that space, then push the next window to the right across too, and so on. This
behaviour will make rearranging even very complex layouts much simpler.

Ideas for savestate selection window:
-The user should be able to click on a savestate slot to select it
-A rightclick context menu should be provided. The user should be able to delete
savestates from a slot through the menu, as well as access additional info about the
savestate.
-An interface needs to be provided which allows the "Comments" metadata to be viewed
and modified. I would suggest the user should be able to rightclick on a slot, and
select an option which will bring up a window where they can enter this information.
-Moving the mouse over the image in a slot which has a savestate loaded into it should
bring up a floating details window, which contains a larger version of the screenshot,
as well as the comments for the savestate, and any additional information we choose to
add later. This interface should be the same one which is displayed to edit the
comments.
-The user should be able to rearrange existing savestates by dragging them around in
the slots.
\*--------------------------------------------------------------------------------------*/
#ifndef __EXODUSINTERFACE_H__
#define __EXODUSINTERFACE_H__
#include <WindowFunctions/WindowFunctions.pkg>
#include "SystemInterface/SystemInterface.pkg"
#include "ISystemExternal.h"
#include "Image/Image.pkg"
#include "ThreadLib/ThreadLib.pkg"
#include "IViewModelManager.h"
#include "ViewModelNotifier.h"
#include <set>
#include <map>
#include <list>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include "MenuSubmenu.h"

class ExodusInterface :public IViewModelManager, public IGUIExtensionInterface
{
public:
	//Constructors
	//##TODO## Implement a destructor, which cleans up our views.
	ExodusInterface(ISystemExternal& asystem);
	virtual ~ExodusInterface();
	HWND CreateMainInterface(HINSTANCE hinstance);
	bool InitializeSystem();

	//Interface version functions
	virtual unsigned int GetIViewModelLauncherVersion() const;
	virtual unsigned int GetIGUIExtensionInterfaceVersion() const;

	//Main window functions
	virtual void* GetMainWindowHandle() const;

	//Savestate functions
	void LoadState(const std::wstring& folder, bool debuggerState);
	void LoadStateFromFile(const std::wstring& filePath, ISystemExternal::FileType fileType, bool debuggerState);
	void SaveState(const std::wstring& folder, bool debuggerState);
	void SaveStateToFile(const std::wstring& filePath, ISystemExternal::FileType fileType, bool debuggerState);

	//Savestate quick-select popup functions
	void QuickLoadState(bool debuggerState);
	void QuickSaveState(bool debuggerState);
	void IncrementSaveSlot();
	void DecrementSaveSlot();
	void SelectSaveSlot(unsigned int slotNo);

	//Workspace functions
	void LoadWorkspace(const std::wstring& folder);
	bool LoadWorkspaceFromFile(const std::wstring& filePath);
	void SaveWorkspace(const std::wstring& folder);
	bool SaveWorkspaceToFile(const std::wstring& filePath);

	//Module functions
	bool CanModuleBeLoaded(const std::wstring& filePath) const;
	bool LoadModule(const std::wstring& folder);
	bool LoadModuleFromFile(const std::wstring& filePath);
	bool SaveSystem(const std::wstring& folder);
	bool SaveSystemToFile(const std::wstring& filePath);
	virtual void UnloadModule(unsigned int moduleID);
	virtual void UnloadAllModules();

	//Global preference functions
	bool LoadPrefs(const std::wstring& filePath);
	void SavePrefs(const std::wstring& filePath);
	void ResolvePrefs();
	virtual bool GetGlobalPreferenceEnableThrottling() const;
	virtual bool GetGlobalPreferenceRunWhenProgramModuleLoaded() const;
	virtual bool GetEnablePersistentState() const;

	//Assembly functions
	bool LoadAssembliesFromFolder(const std::wstring& folder);
	bool LoadAssembly(const std::wstring& filePath);

	//File selection functions
	virtual bool SelectExistingFile(const std::wstring& selectionTypeString, const std::wstring& defaultExtension, const std::wstring& initialFilePath, const std::wstring& initialDirectory, bool scanIntoArchives, std::wstring& selectedFilePath) const;
	virtual bool SelectNewFile(const std::wstring& selectionTypeString, const std::wstring& defaultExtension, const std::wstring& initialFilePath, const std::wstring& initialDirectory, std::wstring& selectedFilePath) const;
	virtual std::vector<std::wstring> PathSplitElements(const std::wstring& path) const;
	virtual Stream::IStream* OpenExistingFileForRead(const std::wstring& path) const;
	virtual void DeleteFileStream(Stream::IStream* stream) const;

	//View management functions
	virtual bool OpenViewModel(IViewModel* aviewModel, bool waitToClose = true, bool openHidden = false);
	virtual void CloseViewModel(IViewModel* aviewModel, bool waitToClose = true);
	virtual void ShowViewModel(IViewModel* aviewModel);
	virtual void HideViewModel(IViewModel* aviewModel);
	virtual void ActivateViewModel(IViewModel* aviewModel);
	virtual bool WaitUntilViewModelOpened(IViewModel* aviewModel);
	virtual void WaitUntilViewModelClosed(IViewModel* aviewModel);
	virtual void NotifyModelViewClosed(IViewModel* aviewModel);

protected:
	//Module functions
	virtual bool CanModuleBeLoadedInternal(const wchar_t* filePath) const;
	virtual bool LoadModuleFromFileInternal(const wchar_t* filePath);

	//Global preference functions
	virtual const wchar_t* GetGlobalPreferencePathModulesInternal() const;
	virtual const wchar_t* GetGlobalPreferencePathSavestatesInternal() const;
	virtual const wchar_t* GetGlobalPreferencePathPersistentStateInternal() const;
	virtual const wchar_t* GetGlobalPreferencePathWorkspacesInternal() const;
	virtual const wchar_t* GetGlobalPreferencePathCapturesInternal() const;
	virtual const wchar_t* GetGlobalPreferencePathAssembliesInternal() const;
	virtual const wchar_t* GetGlobalPreferenceInitialSystemInternal() const;
	virtual const wchar_t* GetGlobalPreferenceInitialWorkspaceInternal() const;

	//Assembly functions
	virtual bool LoadAssemblyInternal(const wchar_t* filePath);

	//File selection functions
	virtual bool SelectExistingFileInternal(const wchar_t* selectionTypeString, const wchar_t* defaultExtension, const wchar_t* initialFilePath, const wchar_t* initialDirectory, bool scanIntoArchives, const wchar_t** selectedFilePath) const;
	virtual bool SelectNewFileInternal(const wchar_t* selectionTypeString, const wchar_t* defaultExtension, const wchar_t* initialFilePath, const wchar_t* initialDirectory, const wchar_t** selectedFilePath) const;
	virtual const wchar_t** PathSplitElementsInternal(const wchar_t* path, unsigned int& arraySize) const;
	virtual void PathSplitElementsInternalFreeArray(const wchar_t** itemArray, unsigned int arraySize) const;
	virtual Stream::IStream* OpenExistingFileForReadInternal(const wchar_t* path) const;

private:
	//Enumerations
	enum ViewOperationType;

	//Structures
	struct SystemPrefs
	{
		std::wstring pathModules;
		std::wstring pathModulesRaw;
		std::wstring pathSavestates;
		std::wstring pathSavestatesRaw;
		std::wstring pathPersistentState;
		std::wstring pathPersistentStateRaw;
		std::wstring pathWorkspaces;
		std::wstring pathWorkspacesRaw;
		std::wstring pathCaptures;
		std::wstring pathCapturesRaw;
		std::wstring pathAssemblies;
		std::wstring pathAssembliesRaw;
		std::wstring loadSystem;
		std::wstring loadSystemRaw;
		std::wstring loadWorkspace;
		std::wstring loadWorkspaceRaw;
		bool enableThrottling;
		bool runWhenProgramModuleLoaded;
		bool enablePersistentState;
	};
	struct NewMenuItem;
	struct ViewInfo;
	struct Region2D;
	struct SavestateCellWindowState;
	struct ViewOperation;
	struct WorkspaceViewEntryDetails;
	struct MapConnectorDialogParams;
	struct SelectCompressedFileDialogParams;
	struct SelectCompressedFileDialogParamsFileEntry;
	struct RegisteredDeviceInfo;
	struct RegisteredExtensionInfo;

	//Typedefs
	typedef std::map<unsigned int, NewMenuItem> NewMenuList;
	typedef std::pair<unsigned int, NewMenuItem> NewMenuListEntry;
	typedef std::map<IViewModel*, ViewInfo*> ViewInfoSet;
	typedef std::pair<IViewModel*, ViewInfo*> ViewInfoSetEntry;
	typedef std::map<IViewModel*, ViewInfo*> ViewModels;
	typedef std::pair<IViewModel*, ViewInfo*> ViewModelsEntry;
	typedef std::list<ViewOperation> ViewOperationQueue;

private:
	//Savestate functions
	std::wstring GetSavestateAutoFileNamePrefix() const;

	//Savestate quick-select popup functions
	void UpdateSaveSlots();

	//Module functions
	void UpdateModuleDisplayInfo() const;

	//File selection functions
	bool SelectExistingFileScanIntoArchive(const std::list<FileSelectionType>& selectionTypes, const std::wstring archivePath, std::wstring& selectedFilePath) const;

	//View management functions
	void FlagProcessPendingEvents();
	void ProcessPendingEvents();
	void ProcessOpenView(IViewModel* viewModel, ViewInfo* viewInfo);
	void ProcessCloseView(IViewModel* viewModel, ViewInfo* viewInfo);
	void ProcessDeleteView(IViewModel* viewModel, ViewInfo* viewInfo);
	void ProcessActivateView(IViewModel* viewModel, ViewInfo* viewInfo);
	void ProcessShowView(IViewModel* viewModel, ViewInfo* viewInfo);
	void ProcessHideView(IViewModel* viewModel, ViewInfo* viewInfo);

	//View closing helper functions
	void CloseViewsForDevice(unsigned int moduleID, const std::wstring& deviceInstanceName);
	void CloseViewsForModule(unsigned int moduleID, bool closeDependentModuleViews);
	void CloseViewsForSystem();
	void CloseAllViews();

	//Child window selection functions
	void BuildActiveWindowList();

	//Menu functions
	bool BuildMenuRecursive(HWND parentWindow, HMENU parentMenu, IMenuItem& amenuItem, unsigned int& nextMenuID, int& insertPos);
	bool BuildFileMenu();
	bool BuildSystemMenu();
	bool BuildSettingsMenu();
	bool BuildDebugMenu();

	//Thread handlers
	void UnloadModuleThread(unsigned int moduleID);
	void UnloadSystemThread();
	void DestroySystemInterfaceThread();

	//Window auto-position functions
	void GetNewWindowPosition(int newWindowWidth, int newWindowHeight, int& newWindowPosX, int& newWindowPosY);
	bool IntersectRegion(const Region2D& existingRegion, const Region2D& regionToIntersect, std::list<Region2D>& newRegionsToCreate) const;
	bool PointWithinRegion(int posx, int posy, const Region2D& region) const;
	bool RegionIntersectsHorizontalLine(int posx, int posy, int width, const Region2D& region) const;
	bool RegionIntersectsVerticalLine(int posx, int posy, int height, const Region2D& region) const;

	//Window callbacks
	static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam);
	static INT_PTR CALLBACK MapConnectorProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
	static INT_PTR CALLBACK LoadModuleProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
	static INT_PTR CALLBACK UnloadModuleProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
	static INT_PTR CALLBACK ModuleManagerProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
	static INT_PTR CALLBACK AboutProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
	static INT_PTR CALLBACK SettingsProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
	static INT_PTR CALLBACK SelectCompressedFileProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK WndSavestateCellProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK WndSavestateProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK WndWindowSelectProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
	ISystemExternal& system;
	HMENU fileMenu;
	int fileMenuNonDynamicMenuItemCount;
	HMENU systemMenu;
	int systemMenuFirstItemIndex;
	HMENU settingsMenu;
	int settingsMenuFirstItemIndex;
	HMENU debugMenu;
	int debugMenuFirstItemIndex;
	HWND moduleManagerDialog;
	MenuSubmenu* fileSubmenu;
	MenuSubmenu* systemSubmenu;
	MenuSubmenu* settingsSubmenu;
	MenuSubmenu* debugSubmenu;
	NewMenuList newMenuList;
	unsigned int nextFreeMenuID;
	SystemPrefs prefs;
	std::wstring originalWorkingDir;
	volatile bool moduleCommandComplete;
	bool systemLoaded;
	bool systemDestructionInProgress;
	bool viewEventProcessingPaused;

	std::list<RegisteredDeviceInfo> registeredDevices;
	std::list<RegisteredExtensionInfo> registeredExtensions;

	HWND mainWindowHandle;
	int mainWindowPosX;
	int mainWindowPosY;
	unsigned int childWindowXPosDefault;
	unsigned int childWindowYPosDefault;
	static const unsigned int childWindowXPosIncrement = 18;
	static const unsigned int childWindowYPosIncrement = 18;

	static const unsigned int cellCount = 10;
	std::vector<SavestateCellWindowState> cell;
	HWND savestatePopup;
	unsigned int selectedSaveCell;
	unsigned int savestatePopupTimeout;
	int savestateMonitorPosX;
	int savestateMonitorPosY;
	int savestateMonitorWidth;
	int savestateMonitorHeight;

	HWND windowSelectHandle;
	std::vector<IViewModel*> activeWindowList;
	unsigned int selectedWindow;
	unsigned int windowSelectColumns;
	unsigned int windowSelectEntriesPerColumn;

	//About dialog state
	HFONT aboutDialogHFont;

	mutable boost::mutex viewMutex;
	bool viewOperationPending;
	ViewOperationQueue viewOperationQueue;
	//##TODO## Unify these two containers. Using a simple boolean flag to indicate whether
	//the view is open would allow us to use a single container.
	ViewInfoSet viewInfoSet;
	ViewModels viewModels;

	mutable std::wstring filePathCache;
};

#include "ExodusInterface.inl"
#endif
