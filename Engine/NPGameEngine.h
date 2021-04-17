#pragma once

#include "d3dApp.h"
#include "BaseObject.h"
#include "LightHelper.h"

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

protected:
	DirectionalLight mDirLight;
	PointLight mPointLight;

	ID3D11RasterizerState* mWireframeRS;
	ID3D11RasterizerState* mSolidRS;

	XMFLOAT4X4 mView;
	XMFLOAT4X4 mProj;
	XMFLOAT3 mEyePosW;

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

