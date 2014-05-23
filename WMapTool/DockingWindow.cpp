#include "stdafx.h"
#include "DockingWindow.h"
#include "Application.h"
#include "resource.h"
#include "Preferences.h"
#include "DockingWindowRegistry.h"
DockingWindow::DockingWindow()
{
}

DockingWindow::~DockingWindow()
{
}

void DockingWindow::Initialize(LPCWSTR className, int titleID, LPCWSTR preferencesPrefix)
{
	Application *application = Application::Instance();
	ATOM atom = RegisterPrivateClass(&DockingWindow::WndProc, className);
	LoadString(application->hInstance, titleID, szTitle, MAX_LOADSTRING);
	Preferences preferences;
	int xPos, yPos;
	int width, height;
	wchar_t xPosName[20], yPosName[20], WidthName[20], HeightName[20];
	wsprintf(xPosName, L"%s%s", preferencesPrefix, L"xPos");
	wsprintf(yPosName, L"%s%s", preferencesPrefix, L"yPos");
	wsprintf(WidthName, L"%s%s", preferencesPrefix, L"Width");
	wsprintf(HeightName, L"%s%s", preferencesPrefix, L"Height");
	prefPrefix = preferencesPrefix;
	xPos = preferences.GetNumericPreference(xPosName);
	yPos = preferences.GetNumericPreference(yPosName);
	width = preferences.GetNumericPreference(WidthName);
	height = preferences.GetNumericPreference(HeightName);

	hWnd = CreateWindow(className, szTitle, WS_CHILD | WS_TILED | WS_CAPTION | WS_THICKFRAME | WS_VISIBLE, xPos, yPos, width, height, application->hWnd, (HMENU)1, application->hInstance, 0);
	if (hWnd == NULL)
	{
		DWORD lastError = GetLastError();
		wchar_t message[100];
		wsprintf(message, L"Could not create %s Window", preferencesPrefix);
		MessageBox(application->hWnd, message, L"Error", MB_ICONERROR);
		return;
	}
	DockingWindowRegistry *windowRegistry = DockingWindowRegistry::Instance();
	windowRegistry->RegisterWindow(hWnd, this);
	CREATESTRUCT createStruct;
	createStruct.cx = width;
	createStruct.cy = height;
	onCreate(hWnd,&createStruct);
	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);
}

LRESULT DockingWindow::onDestroy()
{
	Preferences preferences;
	RECT location;

	GetWindowRect(hWnd, &location);
	MapWindowPoints(HWND_DESKTOP, GetParent(hWnd), (LPPOINT)&location, 2);
	wchar_t xPosName[20], yPosName[20], WidthName[20], HeightName[20];
	wsprintf(xPosName, L"%s%s", prefPrefix, L"xPos");
	wsprintf(yPosName, L"%s%s", prefPrefix, L"yPos");
	wsprintf(WidthName, L"%s%s", prefPrefix, L"Width");
	wsprintf(HeightName, L"%s%s", prefPrefix, L"Height");
	preferences.SaveNumericPreference(xPosName, location.left);
	preferences.SaveNumericPreference(yPosName, location.top);
	preferences.SaveNumericPreference(WidthName, location.right - location.left);
	preferences.SaveNumericPreference(HeightName, location.bottom - location.top);
	PostQuitMessage(0);
	return 0;
}

ATOM DockingWindow::RegisterPrivateClass(WNDPROC wndProc, LPCWSTR className)
{
	WNDCLASSEX wcex;
	Application *application = Application::Instance();
	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = wndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = application->hInstance;
	wcex.hIcon = LoadIcon(application->hInstance, MAKEINTRESOURCE(IDI_WMAPTOOL));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCE(IDC_WMAPTOOL);
	wcex.lpszClassName = className;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

LRESULT CALLBACK DockingWindow::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	DockingWindowRegistry *windowRegistry = DockingWindowRegistry::Instance();
	DockingWindow* dockingWindow = windowRegistry->GetDockingWindow(hWnd);
	if (dockingWindow == 0)  //window being created, hasnt registered yet.
	{
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return dockingWindow->BaseWndProc(message, wParam, lParam);
}

LRESULT DockingWindow::BaseWndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		return onDestroy();
		break;
	case WM_WINDOWPOSCHANGING:
		OnWindowPosChanging(reinterpret_cast<WINDOWPOS*>(lParam));
		return 0;
		break;
	case WM_NOTIFY:
		return onNotify(message, wParam, lParam);
	case WM_SIZE:
		return onMove(message, wParam, lParam);
		break;
	case WM_MOVE:
		return onMove(message, wParam, lParam);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
}

void DockingWindow::OnWindowPosChanging(WINDOWPOS* lpwndPos)
{
	//Get Desktop Size
	RECT rectScreen;
	if (SystemParametersInfo(SPI_GETWORKAREA, 0, &rectScreen, 0)==0) return;

	//Screen Offsets for Snap
	int m_nXOffset = 20;
	int m_nYOffset = 20;

	//Min & Max X Position
	if (lpwndPos->x < 0)
		lpwndPos->x = 0;

	else if (lpwndPos->x + lpwndPos->cx > rectScreen.right)
		lpwndPos->x = rectScreen.right - lpwndPos->cx;

	//Min & Max Y Position
	if (lpwndPos->y < 0)
		lpwndPos->y = 0;

	else if (lpwndPos->y + lpwndPos->cy > rectScreen.bottom)
		lpwndPos->y = rectScreen.bottom - lpwndPos->cy;

	int nGapX = 0;
	//X Snap
	if (abs(lpwndPos->x - rectScreen.left) <= m_nXOffset)
	{
		nGapX = lpwndPos->x - rectScreen.left;
		lpwndPos->cx += nGapX;
		lpwndPos->x = rectScreen.left;
	}
	else if (abs(lpwndPos->x + lpwndPos->cx - rectScreen.right) <= m_nXOffset)
	{
		nGapX = rectScreen.right - (lpwndPos->x + lpwndPos->cx);
		lpwndPos->cx += nGapX;
		lpwndPos->x = rectScreen.right - lpwndPos->cx;
	}

	//Y Snap
	if (abs(lpwndPos->y - rectScreen.top) <= m_nYOffset)
		lpwndPos->y = rectScreen.top;
	else if (abs(lpwndPos->y + lpwndPos->cy - rectScreen.bottom) <= m_nYOffset)
		lpwndPos->y = rectScreen.bottom - lpwndPos->cy;
}

LRESULT DockingWindow::onCreate(HWND hWnd, CREATESTRUCT* createStruct)
{
	return 0;
}

LRESULT DockingWindow::onNotify(UINT message, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(hWnd, message, wParam, lParam);
}

LRESULT DockingWindow::onMove(UINT message, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(hWnd, message, wParam, lParam);
}
