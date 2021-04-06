#include "d3dApp.h"

class InitDirect3DApp : public D3DApp
{
public:
	InitDirect3DApp(HINSTANCE hInstance)
	: D3DApp(hInstance)
	, mColor(0, 0, 0, 1)
	{

	}

	virtual void UpdateScene(float dt = 0) override
	{
		auto output = std::to_wstring(mTimer.TotalTime());
		output += L"\n";
		OutputDebugString(output.c_str());
		mColor.r = 255 * abs(sin(2 * mTimer.TotalTime()));
		mColor.g = 255 * abs(sin(3 * mTimer.TotalTime()));
		mColor.b = 255 * abs(sin(4 * mTimer.TotalTime()));
	}

	virtual void DrawScene() override
	{
		assert(mImmediateContext);
		assert(mSwapChain);

		// Clear buffer
		auto clearColor = XMLoadColor(&mColor);
		mImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&clearColor));
		mImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		// Draw
		mImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// Present buffer
		HR(mSwapChain->Present(0, 0));
	}

private:
	XMCOLOR mColor;
};

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
	_In_ PSTR szCmdLine, _In_ int iCmdShow) 
{
	InitDirect3DApp theApp(hInstance);

	if (!theApp.Init())
		return -1;

	return theApp.Run();
}