#pragma once

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

	void OnResize() override;
	void UpdateScene(float dt = 0) override;
	void DrawScene() override;

	void OnMouseDown(WPARAM btnState, int x, int y) override;
	void OnMouseUp(WPARAM btnState, int x, int y) override;
	void OnMouseMove(WPARAM btnState, int x, int y) override;

private:
	void UpdateProjMatrix();
	void UpdateViewMatrix();

private:
	void BuildGeometryBuffers();
	void BuildShaders();
	void BuildVertexlayout();

private:
	ID3D11Buffer* mBoxVertexBuffer;
	ID3D11Buffer* mBoxIndexBuffer;

	ID3DX11Effect* mFX;
	ID3DX11EffectTechnique* mTech;
	ID3DX11EffectMatrixVariable* mfxWorldViewProj;

	ID3D11InputLayout* mInputLayout;

	XMFLOAT4X4 mWorld;
	XMFLOAT4X4 mView;
	XMFLOAT4X4 mProj;

	float mCamTheta;
	float mCamPhi;
	float mCamRadius;
	float mCamFOV; // In angle
	float mCamNear;
	float mCamFar;

	float mMouseSensitivity;
	POINT mLastMousePos;
};

