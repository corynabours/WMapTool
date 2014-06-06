#include "stdafx.h"
#include "IconBrowser.h"
#include "Application.h"
#include "Resource.h"
#include "FreeImage\Dist\FreeImage.h"
#include <d3d9.h>

extern LPDIRECT3D9 d3d;
LPDIRECT3DDEVICE9 d3ddev;

IconBrowser::IconBrowser(HWND hParentWnd, int left, int top, int width, int height)
{
	Application *application = Application::Instance();
	ATOM atom = RegisterImageList();
	libraryImages = CreateWindow(MAKEINTATOM(atom), L"LibraryImageList", WS_CHILD | WS_BORDER | WS_VISIBLE, left, top, width, height, hParentWnd, (HMENU)2, application->hInstance, 0);
	if (libraryImages == NULL)
	{
		DWORD lastError = GetLastError();
		MessageBox(application->hWnd, L"Could not create ListView in Library Window", L"Error", MB_ICONERROR);
	}
	d3ddev = NULL;
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


void IconBrowser::AddImage(LPWSTR filename)//  FIBITMAP* fiBitmap)
{
	int filenamelength = lstrlen(filename);
	char* cFilename = new char[filenamelength + 1];
	size_t converted;
	wcstombs_s(&converted, cFilename, filenamelength + 1, filename, filenamelength);
	FREE_IMAGE_FORMAT format = FreeImage_GetFileType(cFilename, 0);

	FIBITMAP* hBitmap = FreeImage_Load(format, cFilename, 0);
	FreeImage_FlipVertical(hBitmap);
	//FIBITMAP* bmpImage = FreeImage_ConvertToType(hBitmap, FIT_RGBAF, 1);
	FIBITMAP* fiBitmap = FreeImage_ConvertTo32Bits(hBitmap);
	int height = FreeImage_GetHeight(fiBitmap);
	int width = FreeImage_GetWidth(fiBitmap);
	int bitsperpixel = FreeImage_GetBPP(fiBitmap);
	int bytesperpixel = bitsperpixel / 8;
	if (bytesperpixel == 0) bytesperpixel = 1;
	LPDIRECT3DSURFACE9 surface = NULL;
	LPDIRECT3DSURFACE9 backbuffer = NULL;
	LPDIRECT3DTEXTURE9 pTexture = NULL;
	D3DLOCKED_RECT rect;
	RECT imageRect;
	imageRect.top = 0;
	imageRect.left = 0;
	imageRect.right = width;
	imageRect.bottom = height;
	d3ddev->CreateOffscreenPlainSurface(width, height, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &surface, NULL);
	d3ddev->ColorFill(surface, &imageRect, D3DCOLOR_ARGB(255, 255, 255, 255));
	surface->LockRect(&rect, &imageRect, D3DLOCK_DISCARD);
	byte *dest = (byte*)(rect.pBits);

	//byte *bitmap = (byte*)FreeImage_GetBits(fiBitmap);
	int scan_width = FreeImage_GetPitch(fiBitmap);  //pitch is the number of bytes between rows.  rows may have padding, so cant just use width*bpp
	size_t size = sizeof(unsigned char)* width *height * bytesperpixel;
	BYTE *bitmap = (BYTE *)malloc(size);
	FreeImage_ConvertToRawBits(bitmap, fiBitmap, scan_width, 32, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK);
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
	//memcpy(dest, bitmap, size);
	surface->UnlockRect();
	d3ddev->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backbuffer);
	RECT destRect;
	GetNextImageRect(&destRect);
	d3ddev->BeginScene();
	d3ddev->ColorFill(backbuffer, &destRect, D3DCOLOR_ARGB(255, 255, 255, 255));
	d3ddev->StretchRect(surface, &imageRect, backbuffer, &destRect, D3DTEXF_LINEAR);
	d3ddev->EndScene();
	d3ddev->Present(NULL, NULL, NULL, NULL);
	FreeImage_Unload(fiBitmap);
	//FreeImage_Unload(bmpImage);
	FreeImage_Unload(hBitmap);
	surface->Release();
	backbuffer->Release();
	free(bitmap);
}

void IconBrowser::GetNextImageRect(RECT *pRect)
{
	RECT clientRect;
	GetClientRect(libraryImages, &clientRect);
	int imagesPerLine = clientRect.right / 50;
	if (imagesPerLine == 0) imagesPerLine = 1;
	pRect->top = (imagesShown / imagesPerLine) * 50;
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
	return DefWindowProc(hWnd, message, wParam, lParam);
}

void IconBrowser::Move(int left, int top, int right, int bottom)
{
	MoveWindow(libraryImages, left, top, right, bottom, true);
	if (d3ddev == NULL)
		InitGraphicsWindow();
	else
		ResetGraphicsWindow();
	ViewLocation(locationBeingShown);
}