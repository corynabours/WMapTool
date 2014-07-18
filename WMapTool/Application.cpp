#include "stdafx.h"
#include "Application.h"
#include "ChatWindow.h"
#include "Preferences.h"
#include "Library.h"
#include "resource.h"
#include "windowsx.h"

Application* Application::_instance = 0;
/* 
	Application class handles comunications between windows.  
	It handles main application menu events, and loads the other windows.
	Also handle drag-and-drop.
*/

Application::Application(HINSTANCE instance)
{
	hInstance = instance;
}

void Application::InitializeApplication()
{
	//HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &direct2DFactory);
	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_WMAPTOOL, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	Preferences preferences;
	int nCmdShow = preferences.GetNumericPreference(L"Show");
	if (!InitInstance(nCmdShow))
	{
		return;
	}
}

Application* Application::Instance(HINSTANCE instance)
{
	if (_instance == 0)
	{
		_instance = new Application(instance);
		_instance->InitializeApplication();
	}
	return _instance;
}

Application::~Application()
{
//	direct2DFactory->Release();
//	direct2DFactory = NULL;
}

void Application::LoadWindows()
{
	Preferences preferences;
	int show = preferences.GetNumericPreference(L"Show");
	if (show != 0) ShowWindow(hWnd, show);
	ChatWindow* chatWindow = ChatWindow::Instance();
	int showLibrary = preferences.GetNumericPreference(L"ShowLibrary");
	if (showLibrary)
	{
		HMENU menu = GetMenu(hWnd);
		CheckMenuItem(menu, ID_WINDOW_LIBRARY, MF_CHECKED);
		Library * library = Library::Instance();
	}
	CurrentMap = ClientMap::Instance();
}

void Application::ShowLibrary()
{
	Preferences preferences;
	preferences.SaveNumericPreference(L"ShowLibrary", 1);
	Library * library = Library::Instance();
}

void Application::BeginPaint(PAINTSTRUCT &ps)
{
	CurrentMap->BeginPaint(ps);
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM Application::MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WMAPTOOL));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCE(IDC_WMAPTOOL);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}


//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL Application::InitInstance(int nCmdShow)
{
	int xPos, yPos;
	int width, height;
	Preferences preferences;
	xPos = preferences.GetNumericPreference(L"xPos");
	yPos = preferences.GetNumericPreference(L"yPos");
	width = preferences.GetNumericPreference(L"Width");
	height = preferences.GetNumericPreference(L"Height");
	if (xPos < 0 || yPos < 0 || width < 0 || height < 0)
	{
		xPos = 0;
		yPos = 0;
		width = 1000;
		height = 1000;
	}
	hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		xPos, yPos, width, height, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	LoadWindows();
	return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK Application::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	RECT location;
	PAINTSTRUCT ps;
	HDC hdc;
	Application *application = Application::Instance();
	Preferences preferences;

	switch (message)
	{
	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case ID_WINDOW_LIBRARY:
			application->ShowLibrary();
			break;
		case IDM_ABOUT:
			DialogBox(application->hInstance, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, Application::About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		/*hdc = ::BeginPaint(hWnd, &ps);
		if (hWnd == application->hWnd)
			application->BeginPaint(ps);
		::EndPaint(hWnd, &ps);*/
		//SendMessage(ChatWindow::Instance()->hWnd, WM_PAINT, 0, 0);
		break;
	case WM_SIZE:
		switch (wParam)
		{
		case SIZE_MAXIMIZED:preferences.SaveNumericPreference(L"Show", (int)3); break;
		case SIZE_MINIMIZED:break;
		case SIZE_RESTORED:preferences.SaveNumericPreference(L"Show", (int)1); break;
		}
		return DefWindowProc(hWnd, message, wParam, lParam);
	case WM_DESTROY:
		GetWindowRect(application->hWnd, &location);
		preferences.SaveNumericPreference(L"xPos", location.left);
		preferences.SaveNumericPreference(L"yPos", location.top);
		preferences.SaveNumericPreference(L"Width", location.right - location.left);
		preferences.SaveNumericPreference(L"Height", location.bottom - location.top);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}


// Message handler for about box.
INT_PTR CALLBACK Application::About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}


void Application::BeginDrag(HWND hWnd, HBITMAP bitmap, int x, int y, int width, int height)
{
	POINT location;
	location.x = x;
	location.y = y;
	MapWindowPoints(hWnd, HWND_DESKTOP, (LPPOINT)&location, 1);
	MapWindowPoints(HWND_DESKTOP, this->hWnd, (LPPOINT)&location, 1);

	dragImage = ImageList_Create(width, height, ILC_COLOR32, 0, 1);
	ImageList_Add(dragImage, bitmap, NULL);

	ImageList_BeginDrag(dragImage, 0, 0, 0);
	ImageList_DragEnter(this->hWnd, location.x, location.y);
}

void Application::DragMove(HWND hwnd,int x, int y)
{
	POINT location;
	location.x = x;
	location.y = y;
	if (hwnd!=NULL)
		MapWindowPoints(hwnd, HWND_DESKTOP, (LPPOINT)&location, 1);
	MapWindowPoints(HWND_DESKTOP, this->hWnd, (LPPOINT)&location, 1);
	ImageList_DragMove(location.x, location.y);
}

void Application::DragEnd()
{
	ImageList_EndDrag();
	ImageList_DragLeave(this->hWnd);
	ImageList_Destroy(dragImage);
	dragImage = NULL;
}
bool Application::PreTranslateMsg(MSG* msg)
{
	if (dragImage == NULL) return false;
	if (msg->message == WM_MOUSEMOVE)
	{
		int x = GET_X_LPARAM(msg->lParam);
		int y = GET_Y_LPARAM(msg->lParam);
		DragMove(msg->hwnd,x,y);
		return true;
	}
	if (msg->message == WM_NCMOUSEMOVE)
	{
		int x = GET_X_LPARAM(msg->lParam);
		int y = GET_Y_LPARAM(msg->lParam);
		DragMove(NULL, x, y);
		return true;
	}
	if (msg->message == WM_LBUTTONUP)
	{
		DragEnd();
		return true;
	}
	return false;
}