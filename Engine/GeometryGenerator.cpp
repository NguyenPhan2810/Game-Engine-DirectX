#include "GeometryGenerator.h"

void GeometryGenerator::CreateGrid(float lengthX, float lengthZ, UINT m, UINT n, MeshData& meshData)
{
	// I create grid from top-left to bot-right
	//       Z ^            
	//         |            
	//         |       X    
	// ---------------->   
	//         |            
	//         |            
	//         |            

	float halfX = lengthX * 0.5f;
	float halfZ = lengthZ * 0.5f;

#pragma region Generate vertices
	UINT vertCount = m * n;

	const float dx = lengthX / (m - 1);
	const float dz = lengthZ / (n - 1);

	meshData.vertices.resize(vertCount);

	UINT iv = 0; // vertices index
	float z = halfZ;
	for (UINT iz = 0; iz < m; iz++)
	{
		float x = -halfX;
		for (UINT ix = 0; ix < n; ix++)
		{
			auto& vert = meshData.vertices[iv];

			vert.position = XMFLOAT3(x, 0.0, z);

			iv++;
			x += dx;
		}
		z -= dz;
	}
#pragma endregion

#pragma region indices
	// each quad is divided by 2 so multiply by 2
	UINT faceCount = (m - 1) * (n - 1) * 2;

	meshData.indices.resize(faceCount * 3); // each face has 3 vertices

	UINT q = 0; // iterate through each quad
	for (UINT iz = 0; iz < m - 1; ++iz)
	{
		for (UINT ix = 0; ix < n - 1; ++ix)
		{
			// -----
			//   \ |
			//     -
			// first triangle
			meshData.indices[q++] = iz * n + ix;
			meshData.indices[q++] = iz * n + ix + 1;
			meshData.indices[q++] = (iz + 1) * n + ix + 1;
			
			// -
			// | \
			// -----
			// second triangle
			meshData.indices[q++] = (iz + 1) * n + ix + 1;
			meshData.indices[q++] = (iz + 1) * n + ix;
			meshData.indices[q++] = iz * n + ix;
		}
	}
	
	return;
#pragma endregion
}
