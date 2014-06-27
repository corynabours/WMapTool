// WMapTool.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "WMapTool.h"
#include "Application.h"
#include "Server.h"
#include "Commctrl.h"
#include "FreeImage\Dist\FreeImage.h"
#include <d3d9.h>
#define MAX_LOADSTRING 100
LPDIRECT3D9 d3d;

void StartPersonalServer()
{
	Server* personalServer = Server::PersonalServer();
}


int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	InitCommonControls();
	INITCOMMONCONTROLSEX commControls;
	commControls.dwICC = ICC_TREEVIEW_CLASSES | ICC_LISTVIEW_CLASSES;
	commControls.dwSize = sizeof(INITCOMMONCONTROLSEX);
	InitCommonControlsEx(&commControls);
	FreeImage_Initialise();
	d3d = Direct3DCreate9(D3D_SDK_VERSION);
	MSG msg;
	HACCEL hAccelTable;

	Application* application = Application::Instance(hInstance);
	StartPersonalServer();
	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WMAPTOOL));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			if (!application->PreTranslateMsg(&msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}
	FreeImage_DeInitialise();
	return (int) msg.wParam;
}


