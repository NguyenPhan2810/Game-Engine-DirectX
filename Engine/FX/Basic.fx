#include "LightHelper.fx"

cbuffer cbPerFrame
{
    DirectionalLight gDirLights[3];
    float3 gEyePosW;
};

cbuffer cbPerObject
{
    float4x4 gWorld;
    float4x4 gWorldInvTranspose; // Use to transform normal correctly
    float4x4 gWorldViewProj;
    float4x4 gTexTransform; // Transform texture coords
    Material gMaterial;
    bool     gUseTexture;
};

// Nonnumeric values cannot be added to a cbuffer.
Texture2D gDiffuseMap;

SamplerState samAnisotropic
{
    Filter = ANISOTROPIC;
    MaxAnisotropy = 4;
};

struct VertexIn
{
	float3 PosL     : POSITION;
	float3 NormalL  : NORMAL;
    float2 Tex      : TEXCOORD;
};

struct VertexOut
{
    float4 PosH     : SV_POSITION;
    float3 PosW     : POSITION;
    float3 NormalW  : NORMAL;
    float2 Tex      : TEXCOORD;
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
	
	// Transform tex u and v
    vout.Tex = mul(float4(vin.Tex, 0.0f, 1.0f), gTexTransform).xy;
    
	return vout;
}

// Pixel shader
float4 PS(VertexOut pin, uniform int gLightCount) : SV_Target
{ 
	// Interpolated normal can be unnormalized so here normalize it
    pin.NormalW = normalize(pin.NormalW);
	
    float3 toEyeW = normalize(gEyePosW - pin.PosW);
    
    float4 texColor = float4(1, 1, 1, 1);
    [flatten]
    if (gUseTexture)
    {
        texColor = gDiffuseMap.Sample(samAnisotropic, pin.Tex);
    }

    float4 litColor = texColor;
    
    [flatten]
    if (gLightCount > 0)
    {
	    // Start with a sum of zero. 
        float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
        float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
        float4 spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

	    // Sum the light contribution from each light source.
        float4 A, D, S;
	
	    [unroll]
        for (int i = 0; i < gLightCount; ++i)
        {
            ComputeDirectionalLight(gMaterial, gDirLights[i], pin.NormalW, toEyeW, A, D, S);
            ambient += A;
            diffuse += D;
            spec += S;
        }

        litColor = texColor * (ambient + diffuse) + spec;
    }
	
	// Common to take alpha from diffuse material and texture.
    litColor.a = gMaterial.Diffuse.a * texColor.a;

    return litColor;
}

technique11 Light0
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(0)));
    }
}

technique11 Light1
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(1)));
    }
}

technique11 Light2
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(2)));
    }
}

technique11 Light3
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(3)));
    }
}