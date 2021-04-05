#include "d3dApp.h"

D3DApp::D3DApp()
: mDevice(nullptr)
, mImmediateContext(nullptr)
, mFullscreen(false)
, m4xMsaaQuality(0)
, mEnable4xMsaa(true)
, mClientHeight(600)
, mClientWidth(800)
, mClientRefreshRate(144)
{
	InitDirect3D();
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

#pragma endregion

	return true;
}
