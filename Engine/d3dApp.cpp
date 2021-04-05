#include "d3dApp.h"

D3DApp::D3DApp()
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

	return true;
}
