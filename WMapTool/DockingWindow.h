#pragma once
#define MAX_LOADSTRING 100

class DockingWindow
{
private:
	TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
	ATOM RegisterPrivateClass(WNDPROC wndProc, LPCWSTR className);
	LRESULT BaseWndProc(UINT message, WPARAM wParam, LPARAM lParam);
	LPCWSTR prefPrefix;
	LRESULT onDestroy();
	void OnWindowPosChanging(WINDOWPOS* lpwndPos);
protected:
	virtual LRESULT onCreate(HWND hWnd, CREATESTRUCT* createStruct);
	virtual LRESULT onNotify(UINT message, WPARAM wParam, LPARAM lParam);
	virtual LRESULT onMove(UINT message, WPARAM wParam, LPARAM lParam);
	virtual LRESULT onCommand(UINT message, WPARAM wParam, LPARAM lParam);
	void Initialize(LPCWSTR className, int titleID, LPCWSTR preferencesPrefix);
	HWND hWnd;
public:
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	DockingWindow();
	~DockingWindow();
};

