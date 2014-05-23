#include "stdafx.h"
#include "DockingWindowRegistry.h"


DockingWindowRegistry* DockingWindowRegistry::_instance = 0;

DockingWindowRegistry::DockingWindowRegistry()
{
	windowCount = 0;
	windowHandles = 0;
	dockingWindows = 0;
}


DockingWindowRegistry::~DockingWindowRegistry()
{
	if (windowHandles)
		delete windowHandles;
	if (dockingWindows)
		delete dockingWindows;
}

void DockingWindowRegistry::RegisterWindow(HWND hwnd, DockingWindow* dockingWindow)
{
	if (windowCount)
	{
		HWND *tempWindowHandles = new HWND[windowCount + 1];
		for (int index = 0; index < windowCount; index++)
		{
			tempWindowHandles[index] = windowHandles[index];
		}
		delete[] windowHandles;
		windowHandles = tempWindowHandles;

		DockingWindow **tempDockingWindows = new DockingWindow*[windowCount + 1];
		for (int index = 0; index < windowCount; index++)
		{
			tempDockingWindows[index] = dockingWindows[index];
		}
		delete[] dockingWindows;
		dockingWindows = tempDockingWindows;

		windowHandles[windowCount] = hwnd;
		dockingWindows[windowCount] = dockingWindow;
		windowCount++;
	}
	else
	{
		windowCount = 1;
		windowHandles = new HWND[1];
		windowHandles[0] = hwnd;
		dockingWindows = new DockingWindow*[1];
		dockingWindows[0] = dockingWindow;
	}
}

DockingWindow *DockingWindowRegistry::GetDockingWindow(HWND hwnd)
{
	for (int index = 0; index < windowCount; index++)
	{
		if (windowHandles[index] == hwnd)
			return dockingWindows[index];
	}
	return 0;
}

DockingWindowRegistry * DockingWindowRegistry::Instance()
{
	if (_instance == 0)
	{
		_instance = new DockingWindowRegistry();
	}
	return _instance;
}