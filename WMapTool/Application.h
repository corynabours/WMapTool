#pragma once
#include "ClientMap.h"
#include "d2d1.h"
#include "Commctrl.h"

#define MAX_LOADSTRING 100
class Application
{
public:
	static Application* Instance(HINSTANCE instance = NULL);
	void LoadWindows();
	HINSTANCE hInstance;							// current instance
	HWND hWnd;										// main window
	TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
	TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
	ClientMap *CurrentMap;
	//ID2D1Factory* direct2DFactory;
	void BeginPaint(PAINTSTRUCT &ps);
	void ShowLibrary();
	void BeginDrag(HWND hWnd, HBITMAP bitmap, int x, int y, int width, int height);
	bool PreTranslateMsg(MSG* msg);
protected:
	Application(HINSTANCE instance);
	~Application();
private:
	BOOL InitInstance(int nCmdShow);
	ATOM MyRegisterClass(HINSTANCE hInstance);
	void InitializeApplication();
	void DragMove(HWND hwnd, int x, int y);
	void DragEnd();
	LPARAM onSize(WPARAM wParam, LPARAM lParam);
	HIMAGELIST dragImage = NULL;


	static INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
	static Application * _instance;
};

