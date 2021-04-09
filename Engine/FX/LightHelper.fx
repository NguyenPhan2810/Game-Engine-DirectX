struct DirectionalLight
{
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;
	float3 Direction;
	float Pad; // Pad the last float so we can set an array of lights if we wanted.
};

struct Material
{
	float4 ambient;
	float4 diffuse;
	float4 specular; // w = SpecPower
};

//---------------------------------------------------------------------------------------
// Computes the ambient, diffuse, and specular terms in the lighting equation
// from a directional light.  We need to output the terms separately because
// later we will modify the individual terms.
//---------------------------------------------------------------------------------------
void ComputeDirectionalLight(Material mat, DirectionalLight L,
                             float3 normal, float3 toEye,
					         out float4 ambient,
						     out float4 diffuse,
						     out float4 spec)
{
	// Initialize outputs.
    ambient = float4(0, 0, 0, 0);
    diffuse = float4(0, 0, 0, 0);
    spec = float4(0, 0, 0, 0);
	
	// I refer light vector as opposite to the light ray
    float3 lightVec = -L.Direction;
	
	// Add ambient
    ambient = mat.ambient * L.Ambient;
	
	// diffuse and spectacular factor to make sure
	// light ray comes from the same site as eye
    float diffuseFactor = dot(lightVec, normal);
	
	// Flatten to avoid dynamic branching.
    [flatten] if (diffuseFactor > 0.0f)
    {
        float3 v = reflect(L.Direction, normal);
        float specFactor = pow(max(0, dot(v, toEye)), mat.specular.w);

        diffuse = diffuseFactor * (mat.diffuse * L.Diffuse);
        spec = specFactor * (mat.specular * L.Specular);
    }
}