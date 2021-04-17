#include "stdafx.h"
#include "d3dApp.h"
#include <windowsx.h>

// An instance of the app is needed because MainWndProc
// can not access to a method of a class
D3DApp* gd3dApp = nullptr;
LRESULT MainWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return gd3dApp->MsgProc(hWnd, msg, wParam, lParam);
}

D3DApp::D3DApp(HINSTANCE hInstance)
: mhAppInstance(hInstance)
, mhMainWnd(NULL)

, mDevice(nullptr)
, mImmediateContext(nullptr)
, mSwapChain(nullptr)
, mRenderTargetView(nullptr)
, mDepthStencilView(nullptr)
, mDepthStencilBuffer(nullptr)
, mBackBufferFormat(DXGI_FORMAT_B8G8R8A8_UNORM)

, mStartFullscreen(false)
, m4xMsaaQuality(0)
, mEnable4xMsaa(true)
, mClientHeight(720)
, mClientWidth(1280)
, mClientMinWidth(200)
, mClientMinHeight(200)
, mClientRefreshRate(144)
, mMainWindowCaption(L"Game Engine DirectX 11")

, mAppQuit(false)
, mAppPaused(false)
, mAppMinimized(false)
, mAppMaximized(false)
, mAppResizing(false)
, mTimer()
{
	ZeroMemory(&mScreenViewport, sizeof(D3D11_VIEWPORT));

	// Set a pointer to this instance to get message from MainWndProc
	gd3dApp = this;
}

D3DApp::~D3DApp()
{
	// Can't release swap chain in fullscreen
	assert(mSwapChain);
	HR(mSwapChain->SetFullscreenState(FALSE, NULL));

	ReleaseCOM(mSwapChain);
	ReleaseCOM(mDepthStencilView);
	ReleaseCOM(mRenderTargetView);
	ReleaseCOM(mDepthStencilBuffer);

	// Restore default settings
	if (mImmediateContext)
		mImmediateContext->ClearState();

	ReleaseCOM(mDevice);
	ReleaseCOM(mImmediateContext);
}

HINSTANCE D3DApp::AppInst() const
{
	return mhAppInstance;
}

HWND D3DApp::MainWnd() const
{
	return mhMainWnd;
}

float D3DApp::AspectRatio() const
{
	return static_cast<float>(mClientWidth) / mClientHeight;
}

int D3DApp::Run()
{
	MSG msg{ 0 };

	mTimer.Reset();

	float timeSinceLastUpdate = 0;
	float timePerFrame = 1.0f / mClientRefreshRate;
	float deltaTime = 0;

	while (!mAppQuit)
	{
		// Log everything in prev frame
		if (Output.str().length() > 0)
		{
			OutputDebugStringW(Output.str().c_str());
			Output.str(L"");
		}

		// If there are Window messages then process them.
		while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT)
				mAppQuit = true;
		}

		if (mAppPaused)
		{
			continue;
		}

		// Time control
		mTimer.Tick();
		deltaTime = mTimer.DeltaTime();
		timeSinceLastUpdate += deltaTime;


		// Update scene as fast as client refresh rate
		// So that the dt is fixed for physic or something require fixed step time
		if (timeSinceLastUpdate >= timePerFrame)
		{
			UpdateScene(timePerFrame);
			timeSinceLastUpdate -= timePerFrame;
		}
		
		// Draw scene as fast as possible
		// If the draw step takes long time to execute
		// Then UpdateScene will update several times between frames
		DrawScene();
	}

	return msg.wParam;
}

void D3DApp::Pause()
{
	if (!mAppPaused)
	{
		mAppPaused = true;
		mTimer.Stop();
	}
}

void D3DApp::Unpause()
{
	if (mAppPaused)
	{
		mAppPaused = false;
		mTimer.Start();
	}
}

bool D3DApp::IsPaused() const
{
	return mAppPaused;
}

bool D3DApp::Init()
{
	if (!InitMainWindow())
		return false;

	if (!InitDirect3D())
		return false;

	return true;
}

bool D3DApp::InitMainWindow()
{
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = MainWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = mhAppInstance;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = L"D3DWndClassName";

	if (!RegisterClass(&wc))
	{
		MessageBox(0, L"RegisterClass Failed.", 0, 0);
		return false;
	}

	// Compute window rectangle dimensions based on requested client area dimensions.
	RECT R = { 0, 0, mClientWidth, mClientHeight };
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);

	// Calculate width height after adjustment
	int width = R.right - R.left;
	int height = R.bottom - R.top;

	mhMainWnd = CreateWindow(L"D3DWndClassName", mMainWindowCaption.c_str(),
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, mhAppInstance, 0);
	if (!mhMainWnd)
	{
		MessageBox(0, L"CreateWindow Failed.", 0, 0);
		return false;
	}

	ShowWindow(mhMainWnd, SW_SHOW);
	UpdateWindow(mhMainWnd);

	return true;
}

bool D3DApp::InitDirect3D()
{
#pragma region CreateDevice
	UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL featureLevel;
	HRESULT hr = D3D11CreateDevice(
		nullptr, // Use primary display adapter
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr, // Always use hardware for rendering
		createDeviceFlags,
		nullptr, 0, // Default feature array
		D3D11_SDK_VERSION,
		&mDevice,
		&featureLevel,
		&mImmediateContext);

	if (FAILED(hr))
	{
		MessageBox(0, L"D3D11CreateDevice failed.", NULL, 0);
		return false;
	}

	if (featureLevel != D3D_FEATURE_LEVEL_11_0)
	{
		MessageBox(0, L"Direct3D Feature Level 11 unsupported.", 0, 0);
		return false;
	}
#pragma endregion

#pragma region Check 4X MSAA quality support
	HR(mDevice->CheckMultisampleQualityLevels(
		DXGI_FORMAT_R8G8B8A8_UNORM, 4, &m4xMsaaQuality));

	assert(m4xMsaaQuality > 0);
#pragma endregion

#pragma region Create Swap chain

	// Describe the swap chain
	DXGI_SWAP_CHAIN_DESC sd;
	sd.BufferDesc.Width = mClientWidth;
	sd.BufferDesc.Height = mClientHeight;
	sd.BufferDesc.RefreshRate = DXGI_RATIONAL{ mClientRefreshRate, 1 };
	sd.BufferDesc.Format = mBackBufferFormat;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// 4X MSAA
	if (mEnable4xMsaa)
	{
		sd.SampleDesc.Count = 4;
		sd.SampleDesc.Quality = m4xMsaaQuality - 1; // Use maximum quality
	}
	else
	{
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
	}

	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1; // Double buffer
	sd.OutputWindow = mhMainWnd;
	sd.Windowed = !mStartFullscreen;
	// Should consider using modern swap effect
	// https://devblogs.microsoft.com/directx/dxgi-flip-model/
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0; // Default	
	

	// Create swap chain
	IDXGIDevice* dxgiDevice = 0;
	HR(mDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice));

	IDXGIAdapter* dxgiAdapter = 0;
	HR(dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter));

	IDXGIFactory* dxgiFactory = 0;
	HR(dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory));

	HR(dxgiFactory->CreateSwapChain(mDevice, &sd, &mSwapChain));

	// Disable alt_enter
	//HR(dxgiFactory->MakeWindowAssociation(mhMainWnd, DXGI_MWA_NO_ALT_ENTER));

	ReleaseCOM(dxgiFactory);
	ReleaseCOM(dxgiAdapter);
	ReleaseCOM(dxgiDevice);

#pragma endregion

	// The remaining codes are needed for initialization as also for resizing
	// window, So just call it here like the window is resizing from null to existance
	OnResize();

	return true;
}

void D3DApp::OnResize()
{
	assert(mDevice);
	assert(mImmediateContext);
	assert(mSwapChain);

	// Release old views because they hold refs to the back buffers
	// which is going to be destroyed

	ReleaseCOM(mRenderTargetView);
	ReleaseCOM(mDepthStencilBuffer);
	ReleaseCOM(mDepthStencilView);

	// Resize swap chain (Backbuffer) and recreate render target view
	ID3D11Texture2D* backBuffer = nullptr;
	HR(mSwapChain->ResizeBuffers(1, mClientWidth, mClientHeight, mBackBufferFormat, 0));
	HR(mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer));
	HR(mDevice->CreateRenderTargetView(backBuffer, nullptr, &mRenderTargetView));

	// GetBuffer increase COM ref count to back buffer
	// so release it as it is not in need anymore
	ReleaseCOM(backBuffer);

	// Recreate Depth/Stencil buffer and view
	D3D11_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.Width = mClientWidth;
	depthStencilDesc.Height = mClientHeight;
	depthStencilDesc.MipLevels = 1; // For depth/stencil buffer 1 level is enough
	depthStencilDesc.ArraySize = 1; // For depth/stencil buffer 1 texture is needed in 1 texture array
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT; // Only GPU is able to read/write, CPU isn't
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0; // As CPU doesn't have access to this resource in VRAM
	depthStencilDesc.MiscFlags = 0; // Default

	// 4X MSAA -- must match up with swap chain MSAA values
	if (mEnable4xMsaa)
	{
		depthStencilDesc.SampleDesc.Count = 4;
		depthStencilDesc.SampleDesc.Quality = m4xMsaaQuality - 1; // Use maximum quality
	}
	else
	{
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
	}

	HR(mDevice->CreateTexture2D(&depthStencilDesc, nullptr, &mDepthStencilBuffer));
	HR(mDevice->CreateDepthStencilView(mDepthStencilBuffer, nullptr, &mDepthStencilView));

	// Bind the render target view and depth/stencil view created above to the pipeline
	mImmediateContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);

	// Set viewport
	mScreenViewport = D3D11_VIEWPORT{ 0.0f, 0.0f,
		(float)mClientWidth, (float)mClientHeight,
		0.0f, 1.0f };

	mImmediateContext->RSSetViewports(1, &mScreenViewport);
}

void D3DApp::EventWindowResize(WPARAM wParam)
{
	if (wParam == SIZE_MINIMIZED)
	{
		Pause();
		mAppMinimized = true;
		mAppMaximized = false;
	}
	else if (wParam == SIZE_MAXIMIZED)
	{
		OnResize();
		Unpause();
		mAppMinimized = false;
		mAppMaximized = true;
	}
	else if (wParam == SIZE_RESTORED)
	{
		if (mAppMinimized)
		{
			OnResize();
			Unpause();
			mAppMinimized = false;
		}
		else if (mAppMaximized)
		{
			OnResize();
			Unpause();
			mAppMaximized = false;
		}
		else if (mAppResizing)
		{
			// This does nothing because the OS will send enormous continuously
			// events into this due to user resizing the window. So only when 
			// the user's done resizing then call the OnResize() function
		}
		else // API call such as SetWindowPos or mSwapChain->SetFullscreenState.
		{
			OnResize();
		}
	}
}

LRESULT D3DApp::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	// VM_ACTIVATE is sent when window is activated or deactivated.
	// Pause the game when it deactivates and unpause otherwise.
	case WM_ACTIVATE:
	{
		/*if (LOWORD(wParam) == WA_INACTIVE)
			Pause();
		else
			Unpause();*/

		return NULL;
	}
	// WM_SIZE is sent when window is resized
	case WM_SIZE:
	{
		// Update window dimensions
		mClientWidth = LOWORD(lParam);
		mClientHeight = HIWORD(lParam);

		if (mDevice)
			EventWindowResize(wParam);

		return NULL;
	}
	// WM_ENTERSIZEMOVE is sent when user grab the resize bars
	case WM_ENTERSIZEMOVE:
	{
		Pause();
		mAppResizing = true;
		return NULL;
	}
	// WM_EXITSIZEMOVE is sent when user release the resize bars
	// Reset everything based on the new window dimensions
	case WM_EXITSIZEMOVE:
	{
		Unpause();
		mAppResizing = false;
		OnResize();
		return NULL;
	}
	// WM_DESTROY is sent when the window is being destroy (alt-f4 or red X button...)
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return NULL;
	}
	// WM_MENUCHAR is sent when a menu is active and user press a key that does
	// not correspond to anything
	case WM_MENUCHAR:
		// Don't beep when user press alt-enter.
		return MAKELRESULT(0, MNC_CLOSE);

	// Catch this message so to prevent the window from becoming too small.
	case WM_GETMINMAXINFO:
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = mClientMinWidth;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = mClientMinHeight;
		return 0;
	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
		OnMouseDown(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
		OnMouseUp(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_MOUSEMOVE:
		OnMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;

	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
}