#include "LightHelper.fx"

cbuffer cbPerFrame
{
    DirectionalLight gDirLight;
    PointLight gPointLight;
    float3 gEyePosW;
};

cbuffer cbPerObject
{
    float4x4 gWorld;
    float4x4 gWorldInvTranspose; // Use to transform normal correctly
	float4x4 gWorldViewProj;
    Material gMaterial;
};

struct VertexIn
{
	float3 PosL : POSITION;
	float3 NormalL : NORMAL;
};

struct VertexOut
{
    float4 PosH : SV_POSITION;
    float3 PosW : POSITION;
    float3 NormalW : NORMAL;
};

// Vertex shader
VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	
	// Trasform to world space to get normal first
    vout.PosW = mul(float4(vin.PosL, 1.0f), gWorld).xyz;
	
	// Here multiplied by Inverse transpose to transform to world space
	// and keep normal vector perpendicular to it's original surface
    vout.NormalW = mul((float1x3)vin.NormalL, (float3x3) gWorldInvTranspose);
	
	// Transform to homogeneous clip space.
	vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);
	
	// Just pass vertex color into the pixel shader.
    
	return vout;
}

// Pixel shader
float4 PS(VertexOut pin) : SV_Target
{ 
	// Interpolated normal can be unnormalized so here normalize it
    pin.NormalW = normalize(pin.NormalW);
	
    float3 toEyeW = normalize(gEyePosW - pin.PosW);

	// Start with a sum of zero. 
    float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// Sum the light contribution from each light source.
    float4 A, D, S;
	
    ComputeDirectionalLight(gMaterial, gDirLight, pin.NormalW, toEyeW, A, D, S);
    ambient += A;
    diffuse += D;
    spec += S;
	
    ComputePointLight(gMaterial, gPointLight, pin.PosW, pin.NormalW, toEyeW, A, D, S);
    ambient += A;
    diffuse += D;
    spec += S;

    float4 litColor = ambient + diffuse + spec;
	
	// Common to take alpha from diffuse material.
    litColor.a = gMaterial.diffuse.a;

    return litColor;
}

technique11 LightingTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS()));
	}
}
