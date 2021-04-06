#pragma once
#include "d3dUtil.h"
#include "d3dApp.h"

struct Vertex
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
};

class NPGameEngine : public D3DApp
{
public:
	NPGameEngine(HINSTANCE hInstance);
	~NPGameEngine();

	bool Init() override;
	void UpdateScene(float dt = 0) override;
	void DrawScene() override;

private:
	void BuildGeometryBuffers();

private:
	ID3D11Buffer* mBoxVertexBuffer;
	ID3D11Buffer* mBoxIndexBuffer;
};

