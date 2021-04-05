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
, mFullscreen(false)
, m4xMsaaQuality(0)
, mEnable4xMsaa(true)
, mClientHeight(600)
, mClientWidth(800)
, mClientRefreshRate(144)
, mMainWindowCaption()
{
	// Set a pointer to this instance to get message from MainWndProc
	gd3dApp = this;
}

bool D3DApp::Init()
{
	if (InitMainWindow())
		return false;

	if (!InitDirect3D())
		return false;

	return true;
}

LRESULT D3DApp::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

bool D3DApp::InitMainWindow()
{

	// Compute window rectangle dimensions based on requested client area dimensions.
	RECT R = { 0, 0, mClientWidth, mClientHeight };
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
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
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// 4X MSAA
	if (!mEnable4xMsaa)
		sd.SampleDesc = DXGI_SAMPLE_DESC{ 1, 0 };
	else
	{
		sd.SampleDesc.Count = 4;
		sd.SampleDesc.Quality = m4xMsaaQuality - 1; // Use maximum quality
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

	return true;
}
