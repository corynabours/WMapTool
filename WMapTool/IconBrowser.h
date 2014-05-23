#pragma once
#include "FreeImage\Dist\FreeImage.h"

class IconBrowser
{
private:
	int imagesShown = 0;
	HWND libraryImages = NULL;

	void ClearFrame();
	void AddImage(FIBITMAP* fiBitmap);
	void GetNextImageRect(RECT *pRect);
	void InitImagesImageList();
	ATOM RegisterImageList();
	static LRESULT CALLBACK ImageListWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
public:
	void ViewLocation(LPWSTR location);
	void Move(int left, int top, int right, int bottom);
	IconBrowser(HWND hParentWnd, int left, int top, int width, int height);
	~IconBrowser();
};

