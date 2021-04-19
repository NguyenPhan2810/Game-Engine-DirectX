#include "stdafx.h"
#include "Effects.h"

#pragma region Effect
Effect::Effect(ID3D11Device* device, const std::wstring& filename)
{
	DWORD shaderFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	shaderFlags |= D3D10_SHADER_DEBUG;
	shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif

	ID3D10Blob* compiledShader = nullptr;
	ID3D10Blob* compilationMsgs = nullptr;

	HR(D3DX11CompileFromFile(filename.c_str(),
		0, 0, 0,
		"fx_5_0",
		shaderFlags, 0, 0,
		&compiledShader, &compilationMsgs,
		0));

	// Catch messages
	if (compilationMsgs)
	{
		MessageBoxA(0, (char*)compilationMsgs->GetBufferPointer(), 0, 0);
		ReleaseCOM(compilationMsgs);
	}

	HR(D3DX11CreateEffectFromMemory(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(),
		0, device, &mFX));


	// Shader compilation done
	ReleaseCOM(compiledShader);
}

Effect::~Effect()
{
	ReleaseCOM(mFX);
}
#pragma endregion

#pragma region BasicEffects
BasicEffect::BasicEffect(ID3D11Device* device, const std::wstring& filename)
: Effect(device, filename)
{
	Light1Tech = mFX->GetTechniqueByName("Light1");
	Light2Tech = mFX->GetTechniqueByName("Light2");
	Light3Tech = mFX->GetTechniqueByName("Light3");

	Light0TexTech = mFX->GetTechniqueByName("Light0Tex");
	Light1TexTech = mFX->GetTechniqueByName("Light1Tex");
	Light2TexTech = mFX->GetTechniqueByName("Light2Tex");
	Light3TexTech = mFX->GetTechniqueByName("Light3Tex");

	EyePosW				= mFX->GetVariableByName("gEyePosW")->AsVector();
	DirLights			= mFX->GetVariableByName("gDirLights");
	World				= mFX->GetVariableByName("gWorld")->AsMatrix();
	WorldInvTranspose	= mFX->GetVariableByName("gWorldInvTranspose")->AsMatrix();
	WorldViewProj		= mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
	Mat					= mFX->GetVariableByName("gMaterial");
	TexTransform		= mFX->GetVariableByName("gTexTransform")->AsMatrix();
	DiffuseMap			= mFX->GetVariableByName("gDiffuseMap")->AsShaderResource();
}

BasicEffect::~BasicEffect()
{
}
#pragma endregion


#pragma region Effects
BasicEffect*  Effects::BasicFX = 0;

void Effects::InitAll(ID3D11Device* device)
{
	BasicFX = new BasicEffect(device, L"FX/Basic.fx");
}

void Effects::DestroyAll()
{
	SafeDelete(BasicFX);
}
#pragma endregion

