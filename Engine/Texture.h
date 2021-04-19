#pragma once

#include "d3dUtil.h"

class Texture
{
public:
	// Require vertex layouts initialized
	Texture();
	Texture(const std::wstring& filename);
	virtual ~Texture();

	XMFLOAT4X4 TexTransform;
	ID3D11ShaderResourceView* GetDiffuseMapSRV();
	
protected:
	ID3D11ShaderResourceView* mDiffuseMapSRV;
};