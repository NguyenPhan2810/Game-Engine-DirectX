#include "NPGameEngine.h"

NPGameEngine::NPGameEngine(HINSTANCE hInstance)
: D3DApp(hInstance)
{
	mMainWindowCaption = L"NP Game Engine v0.1";
}

NPGameEngine::~NPGameEngine()
{
	ReleaseCOM(mBoxVertexBuffer);
}

bool NPGameEngine::Init()
{
	if (!D3DApp::Init())
		return false;

	BuildGeometryBuffers();
}

void NPGameEngine::UpdateScene(float dt)
{

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

void NPGameEngine::BuildGeometryBuffers()
{
	Vertex cubeVertices[] =
	{
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), (const float*)&Colors::White   },
		{ XMFLOAT3(-1.0f, +1.0f, -1.0f), (const float*)&Colors::Black   },
		{ XMFLOAT3(+1.0f, +1.0f, -1.0f), (const float*)&Colors::Red     },
		{ XMFLOAT3(+1.0f, -1.0f, -1.0f), (const float*)&Colors::Green   },
		{ XMFLOAT3(-1.0f, -1.0f, +1.0f), (const float*)&Colors::Blue    },
		{ XMFLOAT3(-1.0f, +1.0f, +1.0f), (const float*)&Colors::Yellow  },
		{ XMFLOAT3(+1.0f, +1.0f, +1.0f), (const float*)&Colors::Cyan    },
		{ XMFLOAT3(+1.0f, -1.0f, +1.0f), (const float*)&Colors::Magenta }
	};

	// Create vertex buffer
	D3D11_BUFFER_DESC vbd;
	vbd.ByteWidth = sizeof(cubeVertices);
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vInitData;
	vInitData.pSysMem = cubeVertices;

	HR(mDevice->CreateBuffer(&vbd, &vInitData, &mBoxVertexBuffer));
}
