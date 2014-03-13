#include "stdafx.h"
#include "Application.h"
#include "ChatWindow.h"

Application* Application::_instance = 0;

Application::Application()
{
}

Application* Application::Instance()
{
	if (_instance == 0)
		_instance = new Application();
	return _instance;
}

Application::~Application()
{
}

void Application::LoadWindows()
{
	ChatWindow* chatWindow = ChatWindow::Instance();
}
