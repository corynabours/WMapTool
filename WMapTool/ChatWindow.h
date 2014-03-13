#pragma once
#include "Richedit.h"
#define MAX_LOADSTRING 100
class ChatWindow
{
public:
	static ChatWindow* Instance();
	TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
	HWND hWnd;
	void DisplayMessage(wchar_t *message);
protected:
	ChatWindow();
	void Initialize();
	~ChatWindow();
private:
	HWND messagePanel;
	HWND commandPanel;
	static ChatWindow* _instance;
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	ATOM RegisterClass(HINSTANCE hInstance);
	LRESULT onCreate(HWND hWnd, CREATESTRUCT* createStruct);
	LRESULT onMove(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT CommandPanelNotification(MSGFILTER *msgFilter);
	void ProcessCommand();
};

