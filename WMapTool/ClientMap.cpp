#include "stdafx.h"
#include "ClientMap.h"
#include "Application.h"
#include <d3d9.h>
#include "Resource.h"

extern LPDIRECT3D9 d3d;
LPDIRECT3DDEVICE9 clientMapDevice = NULL;
ClientMap* ClientMap::_instance = NULL;

ClientMap::ClientMap()
{
	Application *application = Application::Instance();
	RECT rc;
	GetClientRect(application->hWnd, &rc);
//	D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);
//	application->direct2DFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(),
//		D2D1::HwndRenderTargetProperties(application->hWnd, size), &pRenderTarget);
	ATOM atom = RegisterClientMap();
	myWindow = CreateWindow(MAKEINTATOM(atom), L"ClientMap", WS_CHILD | WS_CLIPSIBLINGS |WS_VISIBLE, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, application->hWnd, (HMENU)1, application->hInstance, 0);
	if (myWindow == NULL)
	{
		DWORD lastError = GetLastError();
		MessageBox(application->hWnd, L"Could not create Client Map Window", L"Error", MB_ICONERROR);
	}
	clientMapDevice = NULL;
}


ATOM ClientMap::RegisterClientMap()
{
	//WNDPROC wndProc, LPCWSTR className
	WNDCLASSEX wcex;
	Application *application = Application::Instance();
	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = ClientMap::ClientMapWndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = application->hInstance;
	wcex.hIcon = LoadIcon(application->hInstance, MAKEINTRESOURCE(IDI_WMAPTOOL));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCE(IDC_WMAPTOOL);
	wcex.lpszClassName = L"ClientMap";
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}
ClientMap* ClientMap::Instance()
{
	if (_instance == NULL)
	{
		_instance = new ClientMap();
		_instance->InitGraphicsWindow();
	}
	return _instance;
}
void ClientMap::InitGraphicsWindow()
{
	Application *application = Application::Instance();
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = application->hWnd;
	DWORD behaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED;
	d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, application->hWnd, behaviorFlags, &d3dpp, &clientMapDevice);
}

void ClientMap::ResetGraphicsWindow()
{
	Application *application = Application::Instance();
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = application->hWnd;
	//d3ddev->Reset(&d3dpp);  //this seems to leave the window in a bad state, next call to create a surface will fail.
	DWORD behaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED;
	clientMapDevice->Release();
	d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, application->hWnd, behaviorFlags, &d3dpp, &clientMapDevice);
}

void ClientMap::ClearFrame()
{
	clientMapDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);
	clientMapDevice->BeginScene();
	clientMapDevice->EndScene();
	clientMapDevice->Present(NULL, NULL, NULL, NULL);
}

ClientMap::~ClientMap()
{
}

void ClientMap::BeginPaint(PAINTSTRUCT &ps)
{
	RECT updateArea;
	if (0 == GetUpdateRect(myWindow, &updateArea, false)) return;
	PAINTSTRUCT paintStruct;
	ClearFrame();
}

LRESULT CALLBACK ClientMap::ClientMapWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_PAINT)
	{
		PAINTSTRUCT ps;
		HDC hdc;
		hdc = ::BeginPaint(hWnd, &ps);
		ClientMap *clientMap = ClientMap::Instance();
		clientMap->BeginPaint(ps);
		::EndPaint(hWnd, &ps);
		return 0;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}