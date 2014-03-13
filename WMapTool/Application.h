#pragma once
#define MAX_LOADSTRING 100
class Application
{
public:
	static Application* Instance();
	void LoadWindows();
	HINSTANCE hInstance;							// current instance
	HWND hWnd;										// main window
	TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
	TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
protected:
	Application();
	~Application();
private:
	static Application * _instance;
};

