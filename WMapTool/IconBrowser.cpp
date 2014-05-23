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
	InitImagesImageList();
}


IconBrowser::~IconBrowser()
{
}

void IconBrowser::ClearFrame()
{
	imagesShown = 0;
	d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(255, 255, 255), 1.0f, 0);
	d3ddev->BeginScene();
	d3ddev->EndScene();
	d3ddev->Present(NULL, NULL, NULL, NULL);
}


void IconBrowser::AddImage(FIBITMAP* fiBitmap)
{
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
	d3ddev->CreateOffscreenPlainSurface(width, height, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &surface, NULL);
	d3ddev->ColorFill(surface, &imageRect, D3DCOLOR_XRGB(255, 255, 255));
	/*HDC surfaceDC;
	surface->GetDC(&surfaceDC);
	COLORREF white = RGB(255, 255, 255);
	HBRUSH whiteBrush = CreateSolidBrush(white);
	FillRect(surfaceDC, &imageRect, whiteBrush);
	DeleteObject(whiteBrush);
	surface->ReleaseDC(surfaceDC);*/
	surface->LockRect(&rect, &imageRect, D3DLOCK_DISCARD);
	byte *dest = (byte*)(rect.pBits);
	byte *bitmap = (byte*)FreeImage_GetBits(fiBitmap);
	size_t size = sizeof(unsigned char)* width *height * bytesperpixel;
	memcpy(dest, bitmap, size);
	surface->UnlockRect();
	d3ddev->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backbuffer);
	RECT destRect;
	GetNextImageRect(&destRect);
	d3ddev->BeginScene();
	d3ddev->ColorFill(backbuffer, &destRect, D3DCOLOR_XRGB(255, 255, 255));
	d3ddev->StretchRect(surface, &imageRect, backbuffer, &destRect, D3DTEXF_LINEAR);
	d3ddev->EndScene();
	d3ddev->Present(NULL, NULL, NULL, NULL);
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

void IconBrowser::InitImagesImageList()
{
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = libraryImages;
	d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, libraryImages, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &d3ddev);
}

void IconBrowser::ViewLocation(LPWSTR location)
{
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
				//HANDLE hBitmap = LoadImage(application->hInstance, filename, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
				int filenamelength = lstrlen(filename);
				char* cFilename = new char[filenamelength + 1];
				size_t converted;
				wcstombs_s(&converted, cFilename, filenamelength + 1, filename, filenamelength);
				FREE_IMAGE_FORMAT format = FreeImage_GetFileType(cFilename, 0);
				if (format != FIF_UNKNOWN)
				{
					FIBITMAP* hBitmap = FreeImage_Load(format, cFilename, 0);
					FreeImage_FlipVertical(hBitmap);
					FIBITMAP* bmpImage = FreeImage_ConvertToType(hBitmap, FIT_BITMAP, 1);
					FIBITMAP* convertedImage = FreeImage_ConvertTo32Bits(bmpImage);
					AddImage(convertedImage);
					imagesShown++;
					FreeImage_Unload(bmpImage);
					FreeImage_Unload(convertedImage);
					FreeImage_Unload(hBitmap);
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
}