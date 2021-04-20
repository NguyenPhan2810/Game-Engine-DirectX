#pragma once

#include "d3dUtil.h"

class Texture
{
public:
	// Require vertex layouts initialized
	Texture(const std::wstring& filename);
	virtual ~Texture();

	ID3D11ShaderResourceView* GetDiffuseMapSRV();
	
protected:
	ID3D11ShaderResourceView* mDiffuseMapSRV;
};