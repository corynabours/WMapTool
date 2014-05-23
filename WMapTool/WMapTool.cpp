// WMapTool.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "WMapTool.h"
#include "Application.h"
#include "Server.h"
#include "Preferences.h"
#include "Commctrl.h"
#include "FreeImage\Dist\FreeImage.h"
#include <d3d9.h>
#define MAX_LOADSTRING 100
LPDIRECT3D9 d3d;
// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	InitCommonControls();
	INITCOMMONCONTROLSEX commControls;
	commControls.dwICC = ICC_TREEVIEW_CLASSES | ICC_LISTVIEW_CLASSES;
	commControls.dwSize = sizeof(INITCOMMONCONTROLSEX);
	InitCommonControlsEx(&commControls);
	FreeImage_Initialise();
	d3d = Direct3DCreate9(D3D_SDK_VERSION);
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	Application* application = Application::Instance();
	LoadString(hInstance, IDS_APP_TITLE, application->szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_WMAPTOOL, application->szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	Preferences preferences;
	nCmdShow = preferences.GetNumericPreference(L"Show");
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WMAPTOOL));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	FreeImage_DeInitialise();
	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;
	Application *application = Application::Instance();
	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WMAPTOOL));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_WMAPTOOL);
	wcex.lpszClassName	= application->szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}


void StartPersonalServer()
{
	Server* personalServer = Server::PersonalServer();
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
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;
   Application* application = Application::Instance();
   int xPos, yPos;
   int width, height;
   Preferences preferences;
   xPos = preferences.GetNumericPreference(L"xPos");
   yPos = preferences.GetNumericPreference(L"yPos");
   width = preferences.GetNumericPreference(L"Width");
   height = preferences.GetNumericPreference(L"Height");
   hWnd = CreateWindow(application->szWindowClass, application->szTitle, WS_OVERLAPPEDWINDOW,
	   xPos, yPos,width, height, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   application->hInstance = hInstance;
   application->hWnd = hWnd;
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);
   application->LoadWindows();
   StartPersonalServer();
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
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case ID_WINDOW_LIBRARY:
			application->ShowLibrary();
			break;
		case IDM_ABOUT:
			DialogBox(application->hInstance, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		//if (hWnd == application->hWnd)
//			application->BeginPaint(ps);
		EndPaint(hWnd, &ps);
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
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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
