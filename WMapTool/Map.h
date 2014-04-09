#pragma once
//  The Map object contains all the information needed for rendering each map in the campaign.
//  Default map (created without parameters) is a simple tiled Grasslands background-image.
//  ClientMap is derived from Map and has all the same info, plus the graphical rendering code.
class Map
{
private:
	bool generalBackgroundIsTiledImage;

public:
	Map();
	~Map();
};

