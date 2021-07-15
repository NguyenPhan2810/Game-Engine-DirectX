#pragma once

#include "d3dUtil.h"
#include "GameTimer.h"

#include <sstream>

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
	virtual void UpdateScene() = 0;
	virtual void FixedUpdateScene() = 0;
	virtual void DrawScene() = 0;
	virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	virtual void OnMouseDown(WPARAM btnState, int x, int y) { }
	virtual void OnMouseUp(WPARAM btnState, int x, int y) { }
	virtual void OnMouseMove(WPARAM btnState, int x, int y) { }

protected:
	bool InitMainWindow();
	bool InitDirect3D();

	void EventWindowResize(WPARAM wParam);

public:
	// Convenience way to log to Output
	// It works like cout but to output debug window each cycle
	std::wstringstream Output;


	static ID3D11Device* GetDevice();
	static ID3D11DeviceContext* GetImmediateContext();

protected:
	HINSTANCE mhAppInstance;
	HWND mhMainWnd;
	UINT m4xMsaaQuality;

	bool mAppPaused;
	bool mAppMinimized;
	bool mAppMaximized;
	bool mAppResizing;
	bool mAppQuit;

	ID3D11Device* mDevice;
	IDXGISwapChain* mSwapChain;
	ID3D11DeviceContext* mImmediateContext;
	ID3D11RenderTargetView* mRenderTargetView;
	ID3D11DepthStencilView* mDepthStencilView;
	ID3D11Texture2D* mDepthStencilBuffer;
	D3D11_VIEWPORT mScreenViewport;

	DXGI_FORMAT mBackBufferFormat;


	// Derived class should initialize this
	std::wstring mMainWindowCaption;
	bool mEnable4xMsaa;
	int mClientWidth;
	int mClientHeight;
	int mClientMinWidth;
	int mClientMinHeight;
	bool mStartFullscreen;
	UINT mClientRefreshRate;
};