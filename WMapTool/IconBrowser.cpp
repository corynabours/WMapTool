#include "stdafx.h"
#include "IconBrowser.h"
#include "Application.h"
#include "Resource.h"
#include "Commctrl.h"
#include "windowsx.h"

extern LPDIRECT3D9 d3d;
LPDIRECT3DDEVICE9 d3ddev;

/*
This file represents a control that displays all the images in a given file location.
Allows drag (but not drop) from the control (select an image and drag it out of the control onto the map).

Known issues:  Should also handle token files that contain images, such as .rptok
*/
IconBrowser* IconBrowser::_instance = NULL;
IconBrowser::IconBrowser(HWND hParentWnd, int left, int top, int width, int height)
{
	Application *application = Application::Instance();
	ATOM atom = RegisterImageList();
	libraryImages = CreateWindow(MAKEINTATOM(atom), L"LibraryImageList", WS_CHILD | WS_BORDER | WS_VISIBLE | WS_CLIPSIBLINGS | WS_VSCROLL, left, top, width, height, hParentWnd, (HMENU)2, application->hInstance, 0);
	if (libraryImages == NULL)
	{
		DWORD lastError = GetLastError();
		MessageBox(application->hWnd, L"Could not create ListView in Library Window", L"Error", MB_ICONERROR);
	}
	d3ddev = NULL;
	_instance = this;
}
IconBrowser* IconBrowser::Instance()
{
	return _instance;
}

void IconBrowser::InitGraphicsWindow()
{
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = libraryImages;
	DWORD behaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED;
	d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, libraryImages, behaviorFlags, &d3dpp, &d3ddev);
}   

void IconBrowser::ResetGraphicsWindow()
{
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = libraryImages;
	//d3ddev->Reset(&d3dpp);  //this seems to leave the window in a bad state, next call to create a surface will fail.
	DWORD behaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED;
	d3ddev->Release();
	d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, libraryImages, behaviorFlags, &d3dpp, &d3ddev);
}


IconBrowser::~IconBrowser()
{
}

void IconBrowser::ClearFrame()
{
	imagesShown = 0;
	d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(255, 255, 255, 255), 1.0f, 0);
	d3ddev->BeginScene();
	d3ddev->EndScene();
	d3ddev->Present(NULL, NULL, NULL, NULL);
}

void IconBrowser::ClearImageList()
{
	if (images == NULL) return;
	LinkedList *nodes = images;
	images = NULL;
	while (nodes->next != NULL)
	{
		FIBITMAP *bitmap = (FIBITMAP *)nodes->data;
		LinkedList* next = nodes->next;
		FreeImage_Unload(bitmap);
		delete nodes;
		nodes = next;
	}
	FIBITMAP *bitmap = (FIBITMAP *)nodes->data;
	FreeImage_Unload(bitmap);
	delete nodes;
}

FIBITMAP* IconBrowser::OpenImageFile(LPWSTR filename)
{
	int filenamelength = lstrlen(filename);
	char* cFilename = new char[filenamelength + 1];
	size_t converted;
	wcstombs_s(&converted, cFilename, filenamelength + 1, filename, filenamelength);
	FREE_IMAGE_FORMAT format = FreeImage_GetFileType(cFilename, 0);
	FIBITMAP* hBitmap = FreeImage_Load(format, cFilename, 0);
	FreeImage_FlipVertical(hBitmap);
	FIBITMAP* fiBitmap = FreeImage_ConvertTo32Bits(hBitmap);
	FreeImage_Unload(hBitmap);
	return fiBitmap;
}
LPDIRECT3DSURFACE9 IconBrowser::GetSurfaceWithImage(FIBITMAP* fiBitmap, RECT* imageRect)
{
	int height = FreeImage_GetHeight(fiBitmap);
	int width = FreeImage_GetWidth(fiBitmap);
	int bitsperpixel = FreeImage_GetBPP(fiBitmap);
	int bytesperpixel = bitsperpixel / 8;
	if (bytesperpixel == 0) bytesperpixel = 1;

	LPDIRECT3DSURFACE9 surface = NULL;
	d3ddev->CreateOffscreenPlainSurface(width, height, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &surface, NULL);

	imageRect->top = 0;
	imageRect->left = 0;
	imageRect->right = width;
	imageRect->bottom = height;
	d3ddev->ColorFill(surface, imageRect, D3DCOLOR_ARGB(255, 255, 255, 255));

	D3DLOCKED_RECT rect;
	surface->LockRect(&rect, imageRect, D3DLOCK_DISCARD);
	int scan_width = FreeImage_GetPitch(fiBitmap);  //pitch is the number of bytes between rows.  rows may have padding, so cant just use width*bpp
	size_t size = sizeof(unsigned char)* width *height * bytesperpixel;
	BYTE *bitmap = (BYTE *)malloc(size);
	FreeImage_ConvertToRawBits(bitmap, fiBitmap, scan_width, 32, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK);
	byte *dest = (byte*)(rect.pBits);
	for (int y = 0; y < height; y++)
	{
		int rowBegin = y*scan_width;
		for (int x = 0; x < width; x++)
		{
			BYTE alpha = bitmap[(rowBegin)+x * 4 * sizeof(unsigned char)+FI_RGBA_ALPHA];
			if (alpha > 0){ //manually handling alpha.  alpha = 0 means transparent, so dont copy that pixel.
				dest[x * 4 * sizeof(unsigned char)+3] = bitmap[(rowBegin)+x * 4 * sizeof(unsigned char)+FI_RGBA_ALPHA];
				dest[x * 4 * sizeof(unsigned char)+2] = bitmap[(rowBegin)+x * 4 * sizeof(unsigned char)+FI_RGBA_RED];
				dest[x * 4 * sizeof(unsigned char)+1] = bitmap[(rowBegin)+x * 4 * sizeof(unsigned char)+FI_RGBA_GREEN];
				dest[x * 4 * sizeof(unsigned char)+0] = bitmap[(rowBegin)+x * 4 * sizeof(unsigned char)+FI_RGBA_BLUE];
			}
		}
		dest = dest + rect.Pitch; //pitch is the number of bytes between rows.  rows may have padding, so cant just use width*bpp
	}
	surface->UnlockRect();
	free(bitmap);
	return surface;
}

void IconBrowser::AddImage(LPWSTR filename)
{
	FIBITMAP* fiBitmap = OpenImageFile(filename);
	if (images == NULL)
		images = new LinkedList(fiBitmap);
	else
		images->AddToEnd(fiBitmap);
	
	RECT imageRect;
	LPDIRECT3DSURFACE9 surface = GetSurfaceWithImage(fiBitmap,&imageRect);
	LPDIRECT3DSURFACE9 backbuffer = NULL;
	d3ddev->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backbuffer);
	RECT destRect;
	GetNextImageRect(&destRect);
	d3ddev->BeginScene();
	d3ddev->ColorFill(backbuffer, &destRect, D3DCOLOR_ARGB(255, 255, 255, 255));
	d3ddev->StretchRect(surface, &imageRect, backbuffer, &destRect, D3DTEXF_LINEAR);
	d3ddev->EndScene();
	d3ddev->Present(NULL, NULL, NULL, NULL);
	backbuffer->Release();
	backbuffer = NULL;
	surface->Release();
}

void IconBrowser::GetNextImageRect(RECT *pRect)
{
	RECT clientRect;
	GetClientRect(libraryImages, &clientRect);
	int imagesPerLine = (clientRect.right - clientRect.left) / 50;
	if (imagesPerLine == 0) imagesPerLine = 1;
	imagesPerRow = imagesPerLine;

	int minimumPosition = (clientRect.bottom - clientRect.top) / 50;

	pRect->top = (imagesShown / imagesPerLine) * 50 - (scrollPos - minimumPosition)*50;
	pRect->left = (imagesShown % imagesPerLine) * 50;
	pRect->right = pRect->left + 50;
	pRect->bottom = pRect->top + 50;
}

void IconBrowser::ViewLocation(LPWSTR location)
{
	if (d3ddev == NULL)
		InitGraphicsWindow();

	if (location != locationBeingShown)
	{
		int locationLength = lstrlen(location);
		if (locationBeingShown != NULL)
			delete locationBeingShown;
		locationBeingShown = (LPWSTR)new wchar_t[locationLength * sizeof(wchar_t)+1];
		memcpy(locationBeingShown, location, locationLength * sizeof(wchar_t));
		locationBeingShown[locationLength] = 0;
	}
	Application *application = Application::Instance();
	ClearFrame();
	ClearImageList();
	wchar_t search_path[200];
	wchar_t filename[200];
	wsprintf(search_path, L"%s\\*.*", location);
	WIN32_FIND_DATA fd;
	HANDLE hFind = ::FindFirstFile(search_path, &fd);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				int len = lstrlen(fd.cFileName);
				int index;
				for (index = len; len > 0 && fd.cFileName[index] != '.'; index--);
				LPWSTR lastWord = &fd.cFileName[index == 0 ? 0 : index + 1];

				wsprintf(filename, L"%s\\%s", location, fd.cFileName);
				int filenamelength = lstrlen(filename);
				char* cFilename = new char[filenamelength + 1];
				size_t converted;
				wcstombs_s(&converted, cFilename, filenamelength + 1, filename, filenamelength);
				FREE_IMAGE_FORMAT format = FreeImage_GetFileType(cFilename, 0);
				if (format != FIF_UNKNOWN)
				{
					AddImage(filename);
					imagesShown++;
				}
			}
		} while (::FindNextFile(hFind, &fd));
	}

	RECT clientRect;
	GetClientRect(libraryImages, &clientRect);
	int minimumNumberOfLines = (clientRect.bottom - clientRect.top) / 50;
	int numberOfLinesShown = (imagesShown / imagesPerRow);
	if (numberOfLinesShown < minimumNumberOfLines) numberOfLinesShown = minimumNumberOfLines;
	SetScrollRange(libraryImages, SB_VERT, minimumNumberOfLines, numberOfLinesShown, true);
	scrollPos = minimumNumberOfLines;
}

void IconBrowser::RefreshLocation()
{
	Application *application = Application::Instance();
	ClearFrame();
	LinkedList *nodes = images;
	while (nodes != NULL)
	{
		FIBITMAP *bitmap = (FIBITMAP *)nodes->data;
		RECT imageRect;
		LPDIRECT3DSURFACE9 surface = GetSurfaceWithImage(bitmap, &imageRect);
		LPDIRECT3DSURFACE9 backbuffer = NULL;
		d3ddev->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backbuffer);
		RECT destRect;
		GetNextImageRect(&destRect);
		d3ddev->BeginScene();
		d3ddev->ColorFill(backbuffer, &destRect, D3DCOLOR_ARGB(255, 255, 255, 255));
		d3ddev->StretchRect(surface, &imageRect, backbuffer, &destRect, D3DTEXF_LINEAR);
		d3ddev->EndScene();
		d3ddev->Present(NULL, NULL, NULL, NULL);
		backbuffer->Release();
		backbuffer = NULL;
		surface->Release();
		nodes = nodes->next;
		imagesShown++;
	}
}


ATOM IconBrowser::RegisterImageList()
{
	//WNDPROC wndProc, LPCWSTR className
	WNDCLASSEX wcex;
	Application *application = Application::Instance();
	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = IconBrowser::ImageListWndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = application->hInstance;
	wcex.hIcon = LoadIcon(application->hInstance, MAKEINTRESOURCE(IDI_WMAPTOOL));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCE(IDC_WMAPTOOL);
	wcex.lpszClassName = L"LibraryImageList";
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

LRESULT CALLBACK IconBrowser::ImageListWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	IconBrowser *iconBrowser = IconBrowser::Instance();
	switch (message)
	{
	case WM_LBUTTONDOWN:
		int x, y;
		x = GET_X_LPARAM(lParam);
		y = GET_Y_LPARAM(lParam);
		BeginDrag(hWnd, x, y);
		break;
	case  WM_PAINT:
		iconBrowser->OnPaint();
		return 0;
	case WM_VSCROLL:
		iconBrowser->OnScroll(wParam);
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

void IconBrowser::OnScroll(WPARAM wParam)
{
	int newScrollPos = scrollPos;
	switch (LOWORD(wParam))
	{
	case SB_PAGEUP:  // user click in the scroll bar shaft above the scroll box.
		newScrollPos -= 5;
		break;
	case SB_PAGEDOWN: // user click in the scroll bar shaft below the scroll box.
		newScrollPos += 5;
		break;
	case SB_LINEUP: //user clicked the up arrow
		newScrollPos -= 1;
		break;
	case SB_LINEDOWN:  //user clicked the down arrow
		newScrollPos += 1;
		break;
	case SB_THUMBPOSITION:  // user dragged the scroll box
		newScrollPos = HIWORD(wParam);
		break;
	}

	RECT clientRect;
	GetClientRect(libraryImages, &clientRect);
	int minimumPosition = (clientRect.bottom - clientRect.top) / 50;
	if (imagesPerRow == 0) imagesPerRow = 1;  //avoid divide by zero.
	int maximumPosition = (imagesShown / imagesPerRow);
	if (maximumPosition < minimumPosition) maximumPosition = minimumPosition;

	newScrollPos = min(newScrollPos, maximumPosition);
	newScrollPos = max(newScrollPos, minimumPosition);
	if (scrollPos == newScrollPos) return;  // no change.
	scrollPos = newScrollPos;
	SetScrollPos(libraryImages, SB_VERT, scrollPos, true);
	RefreshLocation();
}

FIBITMAP *IconBrowser::GetImageAtIndex(int index)
{
	if (images == NULL) return NULL;
	LinkedList *node = images;
	for (int i = 0; i < index; i++)
	{
		if (node->next == NULL) return NULL;
		node = node->next;
	}
	return (FIBITMAP *)node->data;
}

void IconBrowser::BeginDrag(HWND hWnd, int x, int y)
{
	POINT mouse; 
	mouse.x = x;  
	mouse.y = y;
	RECT clientRect;
	GetClientRect(hWnd, &clientRect);
	int imagesPerLine = clientRect.right / 50;
	if (imagesPerLine == 0) imagesPerLine = 1;
	if (x > (imagesPerLine + 1) * 50) return;  // click in border area to the right of the images.
	int numberOfLines = (_instance->imagesShown) / imagesPerLine;
	if (y > (numberOfLines + 1) * 50) return;  // click in area below the images.
	if (y > (numberOfLines * 50))
	{
		//click is in last line, might be to the right of last image.
		int imagesOnLastLine = _instance->imagesShown%imagesPerLine;
		if (x > imagesOnLastLine * 50) return;
	}
	x = x - x % 50;
	y = y - y % 50;
	Application *application = Application::Instance();

	int imageIndex = (y / 50) * imagesPerLine + (x / 50);
	FIBITMAP *bitmap = _instance->GetImageAtIndex(imageIndex);
	FIBITMAP * scaledBitmap = FreeImage_Rescale(bitmap, 50, 50, FILTER_BSPLINE);
	FreeImage_FlipVertical(scaledBitmap);
	HBITMAP hBitmap = _instance->ConvertFIBitmapToHBitmap(hWnd, scaledBitmap);
	application->BeginDrag(hWnd, hBitmap, mouse.x, mouse.y, 50, 50);
	DeleteObject(hBitmap);
	FreeImage_Unload(scaledBitmap);
}


HBITMAP IconBrowser::ConvertFIBitmapToHBitmap(HWND hwnd, FIBITMAP *fiBitmap)
{
	HDC hdc = GetDC(hwnd);
	HBITMAP bitmap = CreateDIBitmap(hdc, FreeImage_GetInfoHeader(fiBitmap), CBM_INIT, FreeImage_GetBits(fiBitmap), FreeImage_GetInfo(fiBitmap), DIB_RGB_COLORS);
	ReleaseDC(hwnd, hdc);
	return bitmap;
}

void IconBrowser::Move(int left, int top, int right, int bottom)
{
	MoveWindow(libraryImages, left, top, right - left, bottom - top, true);

	int imagesPerLine = (right - left) / 50;
	if (imagesPerLine == 0) imagesPerLine = 1;
	if (imagesPerLine == imagesPerRow)  //number hasnt changed, dont need to refresh the window.
		return;
	if (d3ddev == NULL)
		InitGraphicsWindow();
	else
		ResetGraphicsWindow();
	RefreshLocation();
}

void IconBrowser::OnPaint()
{
	if (locationBeingShown == NULL) return;
	RECT updateArea;
	if (0 == GetUpdateRect(libraryImages, &updateArea, false)) return;
	PAINTSTRUCT paintStruct;
	BeginPaint(libraryImages, &paintStruct);
	RefreshLocation();
	EndPaint(libraryImages, &paintStruct);
}