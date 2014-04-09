#include "stdafx.h"
#include "ClientMap.h"
#include "Application.h"

ClientMap::ClientMap()
{
	Application *application = Application::Instance();
	RECT rc;
	GetClientRect(application->hWnd, &rc);
	D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);
	application->direct2DFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(),
		D2D1::HwndRenderTargetProperties(application->hWnd, size), &pRenderTarget);
}

ClientMap::~ClientMap()
{
	pRenderTarget->Release();
}

void ClientMap::BeginPaint(PAINTSTRUCT &ps)
{
	pRenderTarget->BeginDraw();
	pRenderTarget->Clear();
	pRenderTarget->EndDraw();
}