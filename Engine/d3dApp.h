#pragma once

#include "d3dUtil.h"

class D3DApp
{
public:
	D3DApp();

protected:
	bool InitDirect3D();

protected:
	UINT m4xMsaaQuality;

	ID3D11Device* mDevice;
	ID3D11DeviceContext* mImmediateContext;
};