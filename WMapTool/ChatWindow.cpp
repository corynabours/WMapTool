#include "stdafx.h"
#include "ChatWindow.h"
#include "Application.h"
#include "Resource.h"
#include "ServerConnection.h"
#include "Preferences.h"

ChatWindow* ChatWindow::_instance = 0;

ChatWindow::ChatWindow()
{
}

ChatWindow* ChatWindow::Instance()
{
	if (_instance == 0)
	{
		_instance = new ChatWindow();
		_instance->Initialize(L"MapTool-ChatWindow", IDS_CHAT_TITLE, L"Chat");
	}
	return _instance;
}

ChatWindow::~ChatWindow()
{
}

LRESULT ChatWindow::onNotify(UINT message, WPARAM wParam, LPARAM lParam)
{
	MSGFILTER msgFilter;
	msgFilter = *(MSGFILTER *)lParam;
	if (msgFilter.nmhdr.idFrom == 2)
		return CommandPanelNotification((MSGFILTER *)lParam);
	return DefWindowProc(hWnd, message, wParam, lParam);
}

LRESULT ChatWindow::onMove(UINT message, WPARAM wParam, LPARAM lParam)
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
	messagePanel = CreateWindow(TEXT("RICHEDIT50W"), NULL, ES_MULTILINE | ES_READONLY | WS_CHILD | WS_VISIBLE |WS_VSCROLL, 0, 0, createStruct->cx, createStruct->cy-50, hWnd, (HMENU)1, application->hInstance, 0);
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
	commandPanel = CreateWindow(TEXT("RICHEDIT50W"), NULL, ES_MULTILINE | WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER, 0, createStruct->cy - 50, createStruct->cx, 50, hWnd, (HMENU)2, application->hInstance, 0);
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
	GetWindowText(commandPanel, text, nLength);

	//send the message to the server.
	ServerConnection *serverConnection = ServerConnection::Instance();
	serverConnection->Send(text);

	//then clear the command panel
	SetWindowText(commandPanel, L"");
	delete[] text;
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