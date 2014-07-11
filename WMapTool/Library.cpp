#include "stdafx.h"
#include "Library.h"
#include "Application.h"
#include "Preferences.h"
#include "Resource.h"

Library* Library::_instance = NULL;

Library::Library()
{
}

Library::~Library()
{
	DeleteObject(libraryFont);
}

Library * Library::Instance()
{
	if (_instance == NULL)
	{
		_instance = new Library();
		_instance->Initialize(L"MapTool-Library", IDS_LIBRARY_TITLE,L"Library");
	}
	return _instance;
}

void Library::Unload()
{
	if (_instance != NULL)
	{
		DestroyWindow(_instance->hWnd);
		delete _instance;
		_instance = NULL;
	}
}

LRESULT Library::onCreate(HWND hWnd, CREATESTRUCT* createStruct)
{
	Application *application = Application::Instance();
	libraryListBox = CreateWindowEx(0, WC_TREEVIEW, NULL, TVS_LINESATROOT | TVS_HASBUTTONS | TVS_HASLINES | TVS_SINGLEEXPAND | TVS_FULLROWSELECT | WS_CHILD | WS_VISIBLE | WS_BORDER, 0, 0, createStruct->cx, createStruct->cy/2, hWnd, (HMENU)1, application->hInstance, 0);
	if (libraryListBox == NULL)
	{
		DWORD lastError = GetLastError();
		MessageBox(application->hWnd, L"Could not create Tree View in Library Window", L"Error", MB_ICONERROR);
	}
	libraryFont = CreateFont(14, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
		CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, TEXT("Times New Roman"));
	SendMessage(libraryListBox, WM_SETFONT, (WPARAM)libraryFont, (LPARAM)false);
	InitTreeViewDefaultImageList();
	FillListBoxFromRegisteredLocations();

	libraryImages = new IconBrowser(hWnd, 0, createStruct->cy / 2, createStruct->cx, createStruct->cy / 2);
	return 0;
}

void Library::InitTreeViewDefaultImageList()
{
	Application *application = Application::Instance();
	HBITMAP hbmp = NULL;
	if ((himl = ImageList_Create(16, 16, ILC_COLOR24, 1, 1)) == NULL) return;
	hbmp = LoadBitmap(application->hInstance, MAKEINTRESOURCE(IDB_CLOSED_FOLDER));
	if (hbmp == NULL) return;
	closedFolderIndex = ImageList_Add(himl, hbmp, (HBITMAP)NULL);
	DeleteObject(hbmp);
	TreeView_SetImageList(libraryListBox, himl, TVSIL_NORMAL);
}

void Library::FillListBoxFromRegisteredLocations()
{
	Preferences preferences;
	preferences.SetSubFolder(L"Library");
	int numLocations = preferences.GetNumericPreference(L"RegisteredLocations");
	for (int index = 0; index < numLocations; index++)
	{
		wchar_t locationRegistration[255];
		wsprintf(locationRegistration, L"location%d", index);
		LPWSTR location = preferences.GetStringPreference(locationRegistration);
		AddRootLocation(location);
	}
}

void Library::AddRootLocation(LPWSTR location)
{
	int len = lstrlen(location);
	int index;
	for (index = len; len > 0 && location[index] != '\\'; index--);
	LPWSTR lastWord = &location[index==0?0:index+1];

	TVITEM tvi;
	TVINSERTSTRUCT tvins;
	tvi.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_PARAM;
	tvi.pszText = lastWord;
	tvi.cchTextMax = lstrlen(tvi.pszText);
	tvi.iImage = closedFolderIndex;
	tvi.iSelectedImage = closedFolderIndex;
	tvi.lParam = (LPARAM)RecordFolderLocation(location);
	tvins.item = tvi;
	static HTREEITEM hPrev = (HTREEITEM)TVI_FIRST;
	tvins.hInsertAfter = (HTREEITEM)TVI_ROOT;
	tvins.hParent = NULL;
	if (closedFolderIndex == -1) return;
	hPrev = (HTREEITEM)SendMessage(libraryListBox, TVM_INSERTITEM, 0, (LPARAM)&tvins);
	AddSubFolders(location, hPrev);
}

int Library::RecordFolderLocation(LPWSTR location)
{
	if (locationCount)
	{
		LPWSTR *tempLocations = new LPWSTR[locationCount + 1];
		for (int index = 0; index < locationCount; index++)
		{
			tempLocations[index] = locations[index];
		}
		delete[] locations;
		locations = tempLocations;
		locations[locationCount] = location;
		locationCount++;
	}
	else
	{
		locationCount = 1;
		locations = new LPWSTR[1];
		locations[0] = location;
	}
	return locationCount-1;
}

void Library::AddSubFolders(LPWSTR location, HTREEITEM hParent)
{
	int parentLocationLength = lstrlen(location);
	HTREEITEM hPrev = hParent;
	wchar_t search_path[200];
	wsprintf(search_path, L"%s\\*.*", location);
	WIN32_FIND_DATA fd;
	HANDLE hFind = ::FindFirstFile(search_path, &fd);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if ((lstrcmpW(fd.cFileName, L".") != 0) && (lstrcmpW(fd.cFileName, L"..") != 0))
				{
					int subFolderNameLength = lstrlen(fd.cFileName);
					wchar_t *sublocation = new wchar_t[parentLocationLength + subFolderNameLength + 2];
					wsprintf(sublocation, L"%s\\%s", location, fd.cFileName);

					TVITEM tvi;
					TVINSERTSTRUCT tvins;
					tvi.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_PARAM;
					tvi.pszText = fd.cFileName;
					tvi.cchTextMax = lstrlen(tvi.pszText);
					tvi.iImage = closedFolderIndex;
					tvi.iSelectedImage = closedFolderIndex;
					tvi.lParam = (LPARAM)RecordFolderLocation(sublocation);
					tvins.item = tvi;
					tvins.hInsertAfter = hPrev;
					tvins.hParent = hParent;
					hPrev = (HTREEITEM)SendMessage(libraryListBox, TVM_INSERTITEM, 0, (LPARAM)&tvins);
					AddSubFolders(sublocation, hPrev);
				}
			}
		} while (::FindNextFile(hFind, &fd));
		::FindClose(hFind);
	}
}

LRESULT Library::onMove(UINT message, WPARAM wParam, LPARAM lParam)
{
	RECT rect;
	GetClientRect(hWnd, &rect);
	if (libraryListBox != NULL)
		MoveWindow(libraryListBox, 0, 0, rect.right, rect.bottom/2, true);
	if (libraryImages != NULL)
		libraryImages->Move(0, rect.bottom / 2 + 1, rect.right, rect.bottom);
	return DefWindowProc(hWnd, message, wParam, lParam);
}

LRESULT Library::onNotify(UINT message, WPARAM wParam, LPARAM lParam)
{
	int control = LOWORD(wParam);
	NMHDR *nmptr;
	NMTREEVIEW *nmTreeView;
	if (control == 1)
	{
		nmptr = (LPNMHDR)lParam;
		if (nmptr->code == TVN_SELCHANGED)
		{
			nmTreeView = (LPNMTREEVIEW)lParam;
			TreeViewItemSelected(nmTreeView->itemNew.lParam);
		}
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

void Library::TreeViewItemSelected(int index)
{
	LPWSTR location = locations[index];
	libraryImages->ViewLocation(location);
}
