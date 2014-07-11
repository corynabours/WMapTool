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

	HWND libraryListBox = NULL;
	IconBrowser* libraryImages = NULL;
	HFONT libraryFont = NULL;
	void FillListBoxFromRegisteredLocations();
	void AddRootLocation(LPWSTR location);
	void AddSubFolders(LPWSTR location, HTREEITEM hPrev);
	void InitTreeViewDefaultImageList();
	void InitImagesImageList();
	int closedFolderIndex = -1;
	HIMAGELIST himl = NULL;
	HIMAGELIST himlImages = NULL;
	static LRESULT CALLBACK ImageListWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	int RecordFolderLocation(LPWSTR location);
	int locationCount = 0;
	LPWSTR *locations;
	void TreeViewItemSelected(int index);
public:
	static Library *Instance();
	static void Unload();
protected:
	Library();
	~Library();
};

