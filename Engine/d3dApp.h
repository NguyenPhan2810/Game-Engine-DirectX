#pragma once

#include "d3dUtil.h"
#include "GameTimer.h"

class D3DApp
{
public:
	D3DApp(HINSTANCE hInstance);
	virtual ~D3DApp();

	HINSTANCE AppInst()const;
	HWND      MainWnd()const;
	float     AspectRatio()const;

	int Run();
	void Pause();
	void Unpause();
	bool IsPaused() const;
	
	virtual bool Init();
	virtual void OnResize();
	virtual void UpdateScene(float dt = 0) = 0;
	virtual void DrawScene() = 0;
	virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

protected:
	bool InitMainWindow();
	bool InitDirect3D();

	void EventWindowResize(WPARAM wParam);

protected:
	HINSTANCE mhAppInstance;
	HWND mhMainWnd;
	UINT m4xMsaaQuality;

	bool mAppPaused;
	bool mAppMinimized;
	bool mAppMaximized;
	bool mAppResizing;

	GameTimer mTimer;

	ID3D11Device* mDevice;
	IDXGISwapChain* mSwapChain;
	ID3D11DeviceContext* mImmediateContext;
	ID3D11RenderTargetView* mRenderTargetView;
	ID3D11DepthStencilView* mDepthStencilView;
	ID3D11Texture2D* mDepthStencilBuffer;
	D3D11_VIEWPORT mScreenViewport;

	DXGI_FORMAT mBackBufferFormat;

	std::wstring mMainWindowCaption;
	bool mEnable4xMsaa;
	int mClientWidth;
	int mClientHeight;
	bool mFullscreen;
	UINT mClientRefreshRate;
};