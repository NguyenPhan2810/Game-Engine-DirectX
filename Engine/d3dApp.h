#pragma once

#include "d3dUtil.h"

class D3DApp
{
public:
	D3DApp(HINSTANCE hInstance);

	virtual bool Init();
	virtual void OnResize();
	virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

protected:
	bool InitMainWindow();
	bool InitDirect3D();

protected:
	HINSTANCE mhAppInstance;
	HWND mhMainWnd;
	UINT m4xMsaaQuality;

	ID3D11Device* mDevice;
	IDXGISwapChain* mSwapChain;
	ID3D11DeviceContext* mImmediateContext;
	ID3D11RenderTargetView* mRenderTargetView;

	DXGI_FORMAT mBackBufferFormat;

	std::wstring mMainWindowCaption;
	bool mEnable4xMsaa;
	int mClientWidth;
	int mClientHeight;
	bool mFullscreen;
	UINT mClientRefreshRate;
};