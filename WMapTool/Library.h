#pragma once
#include "DockingWindow.h"
#include "Commctrl.h"
#include "FreeImage\Dist\FreeImage.h"
#include "IconBrowser.h"

class Library : DockingWindow
{
private:
	static Library *_instance;
	LRESULT onCreate(HWND hWnd, CREATESTRUCT* createStruct) override;
	LRESULT onMove(UINT message, WPARAM wParam, LPARAM lParam) override;
	LRESULT onNotify(UINT message, WPARAM wParam, LPARAM lParam) override;
	LRESULT onCommand(UINT message, WPARAM wParam, LPARAM lParam) override;

	HWND libraryListBox = NULL;
	IconBrowser* libraryImages = NULL;
	HFONT libraryFont = NULL;
	void FillListBoxFromRegisteredLocations();
	void AddRootLocation(LPWSTR location);
	void AddSubFolders(LPWSTR location, HTREEITEM hPrev);
	void DisplayAddLocationDialog();
	void DisplayPopupMenu();
	void InitTreeViewDefaultImageList();
	void InitImagesImageList();
	void ResetTree();
	void RemoveLocation();
	int closedFolderIndex = -1;
	HIMAGELIST himl = NULL;
	HIMAGELIST himlImages = NULL;
	static LRESULT CALLBACK ImageListWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	int RecordFolderLocation(LPWSTR location, bool isRoot);
	int selectedLocation = 0;
	int locationCount = 0;
	LPWSTR *locations;
	bool *locationIsRoot;
	void TreeViewItemSelected(int index);
public:
	static Library *Instance();
	static void Unload();
protected:
	Library();
	~Library();
};

