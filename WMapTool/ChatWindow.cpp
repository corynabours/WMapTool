#include "stdafx.h"
#include "ChatWindow.h"
#include "Application.h"
#include "Resource.h"
#include "ServerConnection.h"

ChatWindow* ChatWindow::_instance = 0;

ChatWindow::ChatWindow()
{
}

void ChatWindow::Initialize()
{
	Application *application = Application::Instance();
	ATOM atom = RegisterClass(application->hInstance);
	LoadString(application->hInstance, IDS_CHAT_TITLE, szTitle, MAX_LOADSTRING);
	hWnd = CreateWindow(L"MapTool-ChatWindow", szTitle, WS_CHILD | WS_TILED | WS_CAPTION | WS_THICKFRAME | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, 100, 100, application->hWnd, (HMENU)1, application->hInstance, 0);
	if (hWnd == NULL)
	{
		DWORD lastError = GetLastError();
		MessageBox(application->hWnd, L"Could not create Chat Window", L"Error", MB_ICONERROR);
		return;
	}
	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);
}

ChatWindow* ChatWindow::Instance()
{
	if (_instance == 0)
	{
		_instance = new ChatWindow();
		_instance->Initialize();
	}
	return _instance;
}

ChatWindow::~ChatWindow()
{
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM ChatWindow::RegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;
	Application *application = Application::Instance();
	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = &ChatWindow::WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WMAPTOOL));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCE(IDC_WMAPTOOL);
	wcex.lpszClassName = L"MapTool-ChatWindow";
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
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
LRESULT CALLBACK ChatWindow::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	MSGFILTER msgFilter;
	ChatWindow *chatWindow = ChatWindow::Instance();

	switch (message)
	{
	case WM_CREATE:
		return chatWindow->onCreate(hWnd,reinterpret_cast<CREATESTRUCT*>(lParam));
		break;
	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		 //Parse the menu selections:
		/*switch (wmId)
		{
		default:*/
			return DefWindowProc(hWnd, message, wParam, lParam);
		//}
		break;

	case WM_NOTIFY:
		msgFilter = *(MSGFILTER *)lParam;
		if (msgFilter.nmhdr.idFrom == 2)
			return chatWindow->CommandPanelNotification((MSGFILTER *)lParam);
		return DefWindowProc(hWnd, message, wParam, lParam);
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_SIZE:
		return chatWindow->onMove(hWnd, message, wParam, lParam);
		break;
	case WM_MOVE:
		return chatWindow->onMove(hWnd, message, wParam, lParam);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

LRESULT ChatWindow::onMove(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	RECT rect;
	GetClientRect(hWnd, &rect);
	if (messagePanel != NULL)
	{
		MoveWindow(messagePanel, 0, 0, rect.right, rect.bottom - 50, true);
	}
	if (commandPanel != NULL)
		MoveWindow(commandPanel, 0, rect.bottom - 50, rect.right, 50, true);
	return DefWindowProc(hWnd, message, wParam, lParam);
}

LRESULT ChatWindow::onCreate(HWND hWnd, CREATESTRUCT* createStruct)
{
	Application *application = Application::Instance();
	static HMODULE msft_mod = LoadLibrary(L"Msftedit.dll");
	messagePanel = CreateWindow(TEXT("RICHEDIT50W"), NULL, ES_MULTILINE | ES_READONLY | WS_CHILD | WS_VISIBLE |WS_VSCROLL, 0, 0, createStruct->cx, createStruct->cy, hWnd, (HMENU)1, application->hInstance, 0);
	if (messagePanel == NULL)
	{
		DWORD lastError = GetLastError();
		MessageBox(application->hWnd, L"Could not create Message Panel in Chat Window", L"Error", MB_ICONERROR);
	}
	else
	{
		ShowWindow(messagePanel, SW_SHOW);
		SendMessage(messagePanel, EM_AUTOURLDETECT, WPARAM(AURL_ENABLEURL), 0);
	}
	commandPanel = CreateWindow(TEXT("RICHEDIT50W"), NULL, ES_MULTILINE | WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER, 0, 0, createStruct->cx, createStruct->cy, hWnd, (HMENU)2, application->hInstance, 0);
	if (commandPanel == NULL)
	{
		DWORD lastError = GetLastError();
		MessageBox(application->hWnd, L"Could not create Command Panel in Chat Window", L"Error", MB_ICONERROR);
	}
	else
	{
		SendMessage(commandPanel, EM_SETEVENTMASK, 0, ENM_KEYEVENTS);
		ShowWindow(commandPanel, SW_SHOW);
	}
	return 0;
}

LRESULT ChatWindow::CommandPanelNotification(MSGFILTER *msgFilter)
{
	int key;
	switch (msgFilter->msg)
	{
	case WM_KEYDOWN:
		key = msgFilter->wParam;
		if (key == 13) return 1;
		break;
	case WM_KEYUP:
		key = msgFilter->wParam;
		if (key == 13) {
			ProcessCommand();
		}
		break;
	}
	return 0;
}

void ChatWindow::ProcessCommand()
{
	// get the command from the command panel.
	int nLength = GetWindowTextLength(commandPanel);
	TCHAR* text = new TCHAR[nLength+1];
	memset(text, 0, sizeof(TCHAR)* (nLength+1));
	GetWindowText(commandPanel, text, sizeof(TCHAR)*nLength);

	//send the message to the server.
	ServerConnection *serverConnection = ServerConnection::Instance();
	serverConnection->Send(text);

	//then clear the command panel
	SetWindowText(commandPanel, L"");
	delete text;
}

void ChatWindow::DisplayMessage(wchar_t *message)
{
	CHARRANGE cr;
	cr.cpMin = -1;
	cr.cpMax = -1;
	SendMessage(messagePanel, EM_EXSETSEL, 0, (LPARAM)&cr);
	SendMessage(messagePanel, EM_REPLACESEL, 0, (LPARAM)message);
	// send a carriage return as well.
	wchar_t* endOfLine = L"\n";
	SendMessage(messagePanel, EM_EXSETSEL, 0, (LPARAM)&cr);
	SendMessage(messagePanel, EM_REPLACESEL, 0, (LPARAM)endOfLine);
}