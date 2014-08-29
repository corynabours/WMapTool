#pragma once
#include "d2d1.h"
#include "Map.h"
//  The Map object contains all the information needed for rendering each map in the campaign.
//  Default map (created without parameters) is a simple tiled Grasslands background-image.
//  ClientMap handles how to actually render a map.

class ClientMap 
{
private:
	void InitGraphicsWindow();
	void ResetGraphicsWindow();
	void ClearFrame();
	void InvalidateFloatingWindows();
	ATOM RegisterClientMap();
	HWND myWindow;
	static LRESULT CALLBACK ClientMapWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static ClientMap* _instance;
public:
	ID2D1HwndRenderTarget *pRenderTarget;
	void BeginPaint(PAINTSTRUCT &ps);
	static ClientMap* Instance();
	void Resize();
private:
	ClientMap();
	~ClientMap();
};

