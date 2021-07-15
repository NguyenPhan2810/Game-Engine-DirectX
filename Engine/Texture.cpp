#include "stdafx.h"
#include "Texture.h"
#include "d3dApp.h"


Texture::Texture(const std::wstring& filename)
: mDiffuseMapSRV(nullptr)
{
	HR(D3DX11CreateShaderResourceViewFromFile(D3DApp::GetDevice(),
		filename.c_str(), 0, 0, &mDiffuseMapSRV, 0));
}

Texture::~Texture()
{
	ReleaseCOM(mDiffuseMapSRV);
}

ID3D11ShaderResourceView* Texture::GetDiffuseMapSRV()
{
	return mDiffuseMapSRV;
}
