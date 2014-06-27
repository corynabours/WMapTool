#pragma once
#include "FreeImage\Dist\FreeImage.h"
#include "LinkedList.h"
#include <d3d9.h>

class IconBrowser
{
private:
	int scrollPos = 0;
	int imagesShown = 0;
	int imagesPerRow = 0;
	HWND libraryImages = NULL;
	LPWSTR locationBeingShown = NULL;
	LinkedList *images = NULL;
	void ClearFrame();
	void ClearImageList();
	void AddImage(LPWSTR filename);
	void GetNextImageRect(RECT *pRect);
	void InitGraphicsWindow();
	void ResetGraphicsWindow();
	ATOM RegisterImageList();
	void OnPaint(); 
	void RefreshLocation();
	void OnScroll(WPARAM wParam);
	FIBITMAP* OpenImageFile(LPWSTR filename);
	LPDIRECT3DSURFACE9 GetSurfaceWithImage(FIBITMAP* fiBitmap, RECT* imageRect);
	FIBITMAP *GetImageAtIndex(int index);
	HBITMAP IconBrowser::ConvertFIBitmapToHBitmap(HWND hwnd, FIBITMAP *fiBitmap);
	static LRESULT CALLBACK ImageListWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static void BeginDrag(HWND hWnd, int x, int y);
	static IconBrowser* _instance;
public:
	void ViewLocation(LPWSTR location);
	void Move(int left, int top, int right, int bottom);
	IconBrowser(HWND hParentWnd, int left, int top, int width, int height);
	~IconBrowser();
	static IconBrowser* Instance();

};

