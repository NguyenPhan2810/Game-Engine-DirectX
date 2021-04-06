#pragma once
#include "d3dUtil.h"
#include "d3dApp.h"
class NPGameEngine : public D3DApp
{
public:
	NPGameEngine(HINSTANCE hInstance);
	
	virtual void UpdateScene(float dt = 0) override;

	virtual void DrawScene() override;

private:
};

