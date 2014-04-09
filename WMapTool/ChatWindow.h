#pragma once
#include "DockingWindow.h"
#include "Richedit.h"
#define MAX_LOADSTRING 100
class ChatWindow : DockingWindow
{
public:
	static ChatWindow* Instance();
	TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
	void DisplayMessage(wchar_t *message);
protected:
	ChatWindow();
	~ChatWindow();
private:
	HWND messagePanel;
	HWND commandPanel;
	static ChatWindow* _instance;
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT onCreate(HWND hWnd, CREATESTRUCT* createStruct) override;
	LRESULT onMove(UINT message, WPARAM wParam, LPARAM lParam) override;
	LRESULT onNotify(UINT message, WPARAM wParam, LPARAM lParam) override;
	LRESULT CommandPanelNotification(MSGFILTER *msgFilter);
	void ProcessCommand();
};

