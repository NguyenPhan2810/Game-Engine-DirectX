#pragma once

#include "d3dApp.h"
#include "BaseObject.h"

class NPGameEngine : public D3DApp
{
public:
	NPGameEngine(HINSTANCE hInstance);
	~NPGameEngine();

	virtual bool Init() override;
	virtual void OnResize() override;
	virtual void UpdateScene(float dt = 0) override;
	virtual void DrawScene() override;

	virtual void OnMouseDown(WPARAM btnState, int x, int y) override;
	virtual void OnMouseUp(WPARAM btnState, int x, int y) override;
	virtual void OnMouseMove(WPARAM btnState, int x, int y) override;

private:
	void UpdateProjMatrix();
	void UpdateViewMatrix();

protected:
	virtual void InitRasterizerState();

	virtual void BuildGeometryBuffers();
	virtual void BuildShaders();
	virtual void BuildVertexlayout();

protected:
	ID3DX11Effect* mFX;
	ID3DX11EffectTechnique* mTech;
	ID3DX11EffectMatrixVariable* mfxWorldViewProj;
	ID3DX11EffectMatrixVariable* mfxWorld;
	ID3DX11EffectMatrixVariable* mfxWorldInvTranspose;
	ID3DX11EffectVectorVariable* mfxEyePosW;
	ID3DX11EffectVariable* mfxDirLight;
	//ID3DX11EffectVariable* mfxPointLight;
	//ID3DX11EffectVariable* mfxSpotLight;
	ID3DX11EffectVariable* mfxMaterial;

	DirectionalLight mDirLight;

	ID3D11InputLayout* mInputLayout;
	ID3D11RasterizerState* mWireframeRS;
	ID3D11RasterizerState* mSolidRS;

	XMFLOAT4X4 mView;
	XMFLOAT4X4 mProj;

	float mCamTheta;
	float mCamPhi;
	float mCamRadius;
	float mCamFOV; // In angle
	float mCamNear;
	float mCamFar;

	float mMouseSensitivity;

private:
	POINT mLastMousePos;
};

