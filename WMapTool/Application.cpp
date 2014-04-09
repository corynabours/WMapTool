#include "stdafx.h"
#include "Application.h"
#include "ChatWindow.h"
#include "Preferences.h"
#include "Library.h"
#include "resource.h"

Application* Application::_instance = 0;

Application::Application()
{
	HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &direct2DFactory);
}

Application* Application::Instance()
{
	if (_instance == 0)
		_instance = new Application();
	return _instance;
}

Application::~Application()
{
	direct2DFactory->Release();
	direct2DFactory = NULL;
}

void Application::LoadWindows()
{
	Preferences preferences;
	int show = preferences.GetNumericPreference(L"Show");
	if (show != 0) ShowWindow(hWnd, show);
	ChatWindow* chatWindow = ChatWindow::Instance();
	CurrentMap = new ClientMap();
	int showLibrary = preferences.GetNumericPreference(L"ShowLibrary");
	if (showLibrary)
	{
		CheckMenuItem((HMENU)1, ID_WINDOW_LIBRARY, MF_CHECKED);
		Library * library = Library::Instance();
	}
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