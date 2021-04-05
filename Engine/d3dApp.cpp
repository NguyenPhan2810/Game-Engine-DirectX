#include "d3dApp.h"

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
, mScreenViewport()
, mBackBufferFormat(DXGI_FORMAT_B8G8R8A8_UNORM)

, mFullscreen(false)
, m4xMsaaQuality(0)
, mEnable4xMsaa(true)
, mClientHeight(600)
, mClientWidth(800)
, mClientRefreshRate(144)
, mMainWindowCaption(L"Game Engine DirectX 11")
{
	// Set a pointer to this instance to get message from MainWndProc
	gd3dApp = this;
}

bool D3DApp::Init()
{
	if (!InitMainWindow())
		return false;

	if (!InitDirect3D())
		return false;

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

LRESULT D3DApp::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(hwnd, msg, wParam, lParam);
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
		NULL, // Use primary display adapter
		D3D_DRIVER_TYPE_HARDWARE,
		NULL, // Always use hardware for rendering
		createDeviceFlags,
		NULL, 0, // Default feature array
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
	sd.Windowed = !mFullscreen;
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

	ReleaseCOM(dxgiFactory);
	ReleaseCOM(dxgiAdapter);
	ReleaseCOM(dxgiDevice);

#pragma endregion

	// The remaining codes are needed for initialization as also for resizing
	// window, So just call it here like the window is resizing from null to existance
	OnResize();

	return true;
}
