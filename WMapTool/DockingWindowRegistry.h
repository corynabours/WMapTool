#pragma once
#include "DockingWindow.h"
class DockingWindowRegistry
{
private:
	static DockingWindowRegistry* _instance;
	int windowCount;
	HWND *windowHandles;
	DockingWindow** dockingWindows;
public:
	static DockingWindowRegistry* Instance();
	void RegisterWindow(HWND windowHandle, DockingWindow *dockingWindow);
	DockingWindow *GetDockingWindow(HWND hwnd);
protected:
	DockingWindowRegistry();
	~DockingWindowRegistry();
};

