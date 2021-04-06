#include "NPGameEngine.h"

NPGameEngine::NPGameEngine(HINSTANCE hInstance)
: D3DApp(hInstance)
{
	mMainWindowCaption = L"NP Game Engine v0.1";
}

void NPGameEngine::UpdateScene(float dt)
{
	LogOut << "Hello " << dt << "\n";
}

void NPGameEngine::DrawScene()
{
	assert(mImmediateContext);
	assert(mSwapChain);

	// Clear buffer

	mImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::Cyan));
	mImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// Draw
	mImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Present buffer
	HR(mSwapChain->Present(0, 0));
}