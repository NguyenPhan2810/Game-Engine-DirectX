#include "d3dApp.h"

#include <iostream>

class InitDirect3DApp : public D3DApp
{
public:
	InitDirect3DApp(HINSTANCE hInstance)
	: D3DApp(hInstance)
	{

	}

	virtual void UpdateScene(float dt = 0) override
	{

	}

	virtual void DrawScene() override
	{
		assert(mImmediateContext);
		assert(mSwapChain);

		mImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::Blue));
		mImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		HR(mSwapChain->Present(0, 0));
	}
};

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
	_In_ PSTR szCmdLine, _In_ int iCmdShow) 
{
	InitDirect3DApp theApp(hInstance);

	if (!theApp.Init())
		return -1;

	return theApp.Run();
}