#pragma once
#include "d2d1.h"
#include "Map.h"
//  The Map object contains all the information needed for rendering each map in the campaign.
//  Default map (created without parameters) is a simple tiled Grasslands background-image.
//  ClientMap handles how to actually render a map.

class ClientMap 
{
public:
	ID2D1HwndRenderTarget *pRenderTarget;
	void BeginPaint(PAINTSTRUCT &ps);
public:
	ClientMap();
	~ClientMap();
};
