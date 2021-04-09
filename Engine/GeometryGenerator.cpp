#include "GeometryGenerator.h"
#include <map>
#include <fstream>

void GeometryGenerator::ConvertToGlobVertex(std::vector<GeometryGenerator::Vertex>& source, std::vector<GLOBDEF::Vertex>& vertices)
{
	size_t n = source.size();
	vertices.resize(n);
	for (UINT i = 0; i < n; ++i)
	{
		vertices[i].Pos = source[i].position;
		vertices[i].Color = XMFLOAT4(0.5, 0.5, 0.5, 1);
	}

}

void GeometryGenerator::CreateFromFile(const std::wstring& filepath, MeshData& meshData)
{
	std::ifstream fin(filepath);

	if (!fin)
	{
		MessageBox(0, filepath.c_str(), 0, 0);
		return;
	}

	UINT vcount = 0;
	UINT tcount = 0;
	std::string ignore;

	fin >> ignore >> vcount;
	fin >> ignore >> tcount;
	fin >> ignore >> ignore >> ignore >> ignore;

	float nx, ny, nz;

	std::vector<Vertex>& vertices = meshData.vertices;
	vertices.resize(vcount);
	for (UINT i = 0; i < vcount; ++i)
	{
		fin >> vertices[i].position.x >> vertices[i].position.y >> vertices[i].position.z;

		// Normal not used in this demo.
		fin >> nx >> ny >> nz;
	}

	fin >> ignore;
	fin >> ignore;
	fin >> ignore;

	UINT skullIndexCount = 3 * tcount;
	std::vector<UINT>& indices = meshData.indices;
	indices.resize(skullIndexCount);
	for (UINT i = 0; i < tcount; ++i)
	{
		fin >> indices[i * 3 + 0] >> indices[i * 3 + 1] >> indices[i * 3 + 2];
	}

	fin.close();
}

void GeometryGenerator::CreateBox(float width, float height, float depth, MeshData& meshData)
{
	//
	// Create the vertices.
	//

	Vertex v[24];

	float w2 = 0.5f * width;
	float h2 = 0.5f * height;
	float d2 = 0.5f * depth;

	// Fill in the front face vertex data.
	v[0] = Vertex(-w2, -h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[1] = Vertex(-w2, +h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[2] = Vertex(+w2, +h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	v[3] = Vertex(+w2, -h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

	// Fill in the back face vertex data.
	v[4] = Vertex(-w2, -h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	v[5] = Vertex(+w2, -h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[6] = Vertex(+w2, +h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[7] = Vertex(-w2, +h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	// Fill in the top face vertex data.
	v[8] = Vertex(-w2, +h2, -d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[9] = Vertex(-w2, +h2, +d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[10] = Vertex(+w2, +h2, +d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	v[11] = Vertex(+w2, +h2, -d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

	// Fill in the bottom face vertex data.
	v[12] = Vertex(-w2, -h2, -d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	v[13] = Vertex(+w2, -h2, -d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[14] = Vertex(+w2, -h2, +d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[15] = Vertex(-w2, -h2, +d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	// Fill in the left face vertex data.
	v[16] = Vertex(-w2, -h2, +d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[17] = Vertex(-w2, +h2, +d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[18] = Vertex(-w2, +h2, -d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	v[19] = Vertex(-w2, -h2, -d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

	// Fill in the right face vertex data.
	v[20] = Vertex(+w2, -h2, -d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
	v[21] = Vertex(+w2, +h2, -d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
	v[22] = Vertex(+w2, +h2, +d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);
	v[23] = Vertex(+w2, -h2, +d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);

	meshData.vertices.assign(&v[0], &v[24]);

	//
	// Create the indices.
	//

	UINT i[36];

	// Fill in the front face index data
	i[0] = 0; i[1] = 1; i[2] = 2;
	i[3] = 0; i[4] = 2; i[5] = 3;

	// Fill in the back face index data
	i[6] = 4; i[7] = 5; i[8] = 6;
	i[9] = 4; i[10] = 6; i[11] = 7;

	// Fill in the top face index data
	i[12] = 8; i[13] = 9; i[14] = 10;
	i[15] = 8; i[16] = 10; i[17] = 11;

	// Fill in the bottom face index data
	i[18] = 12; i[19] = 13; i[20] = 14;
	i[21] = 12; i[22] = 14; i[23] = 15;

	// Fill in the left face index data
	i[24] = 16; i[25] = 17; i[26] = 18;
	i[27] = 16; i[28] = 18; i[29] = 19;

	// Fill in the right face index data
	i[30] = 20; i[31] = 21; i[32] = 22;
	i[33] = 20; i[34] = 22; i[35] = 23;

	meshData.indices.assign(&i[0], &i[36]);
}

void GeometryGenerator::CreateSphere(float radius, UINT sliceCount, UINT stackCount, MeshData& meshData)
{
	// Generate the rings first then the poles
	
	// phi is the angle int the XY with respect to Y axis
	float dPhi = XM_PI / stackCount;
	float phi = dPhi;

	// stackCount -2 because 2 of the stack connects to poles
	for (UINT iRing = 0; iRing <= stackCount - 2; ++iRing)
	{
		float x = radius * sinf(phi);
		float y = radius * cosf(phi);

		CreateRingVertex(x, y, sliceCount, meshData, true);

		phi += dPhi;
	}

	// Create indices
	UINT ringVertexCount = sliceCount + 1;
	for (UINT iStack = 0; iStack < stackCount - 2; ++iStack)
	{
		for (UINT iSlice = 0; iSlice < sliceCount; ++iSlice)
		{
			meshData.indices.push_back(iStack * ringVertexCount + iSlice);
			meshData.indices.push_back((iStack + 1) * ringVertexCount + iSlice);
			meshData.indices.push_back((iStack + 1) * ringVertexCount + iSlice + 1);

			meshData.indices.push_back((iStack + 1) * ringVertexCount + iSlice + 1);
			meshData.indices.push_back(iStack * ringVertexCount + iSlice + 1);
			meshData.indices.push_back(iStack * ringVertexCount + iSlice);
		}
	}

	// Create poles
	Vertex northPole, southPole;
	northPole.position.x = 0;
	northPole.position.y = radius;
	northPole.position.z = 0;
	southPole.position.x = 0;
	southPole.position.y = -radius;
	southPole.position.z = 0;
	meshData.vertices.push_back(northPole);
	meshData.vertices.push_back(southPole);

	size_t nVertices = meshData.vertices.size();
	// Index north pole
	for (UINT iSlice = 0; iSlice < sliceCount; ++iSlice)
	{
		meshData.indices.push_back(nVertices - 2); // north pole index
		meshData.indices.push_back(iSlice);
		meshData.indices.push_back(iSlice + 1);
	}

	// Index south pole
	// -1 for south pole index -2 to get to the last ring's last vertex
	// -ringVertexCount to get the last ring's first vertex
	UINT baseIndex = nVertices - 1 - 2 - sliceCount;
	for (UINT iSlice = 0; iSlice < sliceCount; ++iSlice)
	{
		meshData.indices.push_back(nVertices - 1); // south pole index
		meshData.indices.push_back(iSlice + baseIndex + 1);
		meshData.indices.push_back(iSlice + baseIndex);
	}
	
	return;
}

void GeometryGenerator::CreateGeoSphere(float radius, UINT nSubdivisions, MeshData& meshData)
{
	// Approximate a sphere by tessellating an icosahedron.

	// Create icosahedron
	const float X = 0.525731f;
	const float Z = 0.850651f;

	XMFLOAT3 pos[12] =
	{
		XMFLOAT3(-X, 0.0f, Z),  XMFLOAT3(X, 0.0f, Z),
		XMFLOAT3(-X, 0.0f, -Z), XMFLOAT3(X, 0.0f, -Z),
		XMFLOAT3(0.0f, Z, X),   XMFLOAT3(0.0f, Z, -X),
		XMFLOAT3(0.0f, -Z, X),  XMFLOAT3(0.0f, -Z, -X),
		XMFLOAT3(Z, X, 0.0f),   XMFLOAT3(-Z, X, 0.0f),
		XMFLOAT3(Z, -X, 0.0f),  XMFLOAT3(-Z, -X, 0.0f)
	};

	DWORD k[60] =
	{
		1,4,0,  4,9,0,  4,5,9,  8,5,4,  1,8,4,
		1,10,8, 10,3,8, 8,3,5,  3,2,5,  3,7,2,
		3,10,7, 10,6,7, 6,11,7, 6,0,11, 6,1,0,
		10,1,6, 11,0,9, 2,11,9, 5,2,9,  11,2,7
	};

	meshData.vertices.resize(12);
	meshData.indices.resize(60);

	for (UINT i = 0; i < 12; ++i)
		meshData.vertices[i].position = pos[i];

	for (UINT i = 0; i < 60; ++i)
		meshData.indices[i] = k[i];

	// Subdivide
	for(int i = 0; i < nSubdivisions; ++i)
		Subdivide(meshData);

	// Project onto a sphere and scale
	size_t nVertices = meshData.vertices.size();
	for (UINT i = 0; i < nVertices; ++i)
	{
		auto& pos = meshData.vertices[i].position;
		// Normalize vector
		XMVECTOR n = XMVector3Normalize(XMLoadFloat3(&pos));

		// Project onto a sphere
		XMVECTOR p = n * radius;
		
		XMStoreFloat3(&pos, p);
	}
}

void GeometryGenerator::Subdivide(MeshData& meshData)
{
	// Save a copy of the input data to reoder later
	MeshData meshDataCopy = meshData;

	meshData.vertices.clear();
	meshData.indices.clear();

	// A visual of how to divide and the indices
	//        v1(1)
	//        *
	//       / \
	//      /   \
	//(3)m0*-----*m1(4)
	//    / \   / \
	//   /   \ /   \
	//  *-----*-----*
	// v0(0)  m2(5)  v2(2)


#define GetIndexedVertex(index) meshDataCopy.vertices[meshDataCopy.indices[index]]

	// Lambda to get new middle vertex based on indices
	auto GetNewVertex = [&](UINT i1, UINT i2, Vertex& newVert)->void
	{
		XMVECTOR p1 = XMLoadFloat3(&GetIndexedVertex(i1).position);
		XMVECTOR p2 = XMLoadFloat3(&GetIndexedVertex(i2).position);
		XMStoreFloat3(&newVert.position, 0.5f * (p1 + p2));
	};

	Vertex m0, m1, m2;

	UINT nVerts = meshDataCopy.indices.size();
	for (int iv = 0; iv < nVerts; iv += 3)
	{
		GetNewVertex(iv, iv + 1, m0);
		GetNewVertex(iv + 1, iv + 2, m1);
		GetNewVertex(iv + 2, iv, m2);

		// Add new vertices
		meshData.vertices.push_back(GetIndexedVertex(iv)); // 0
		meshData.vertices.push_back(GetIndexedVertex(iv + 1)); // 1
		meshData.vertices.push_back(GetIndexedVertex(iv + 2)); // 2
		meshData.vertices.push_back(m0); // 3
		meshData.vertices.push_back(m1); // 4
		meshData.vertices.push_back(m2); // 5

		meshData.indices.push_back(iv * 2 + 0);
		meshData.indices.push_back(iv * 2 + 3);
		meshData.indices.push_back(iv * 2 + 5);

		meshData.indices.push_back(iv * 2 + 3);
		meshData.indices.push_back(iv * 2 + 4);
		meshData.indices.push_back(iv * 2 + 5);

		meshData.indices.push_back(iv * 2 + 4);
		meshData.indices.push_back(iv * 2 + 2);
		meshData.indices.push_back(iv * 2 + 5);

		meshData.indices.push_back(iv * 2 + 3);
		meshData.indices.push_back(iv * 2 + 1);
		meshData.indices.push_back(iv * 2 + 4);
	}
}

void GeometryGenerator::CreateGrid(float lengthX, float lengthZ, UINT m, UINT n, MeshData& meshData)
{
	float halfX = lengthX * 0.5f;
	float halfZ = lengthZ * 0.5f;

#pragma region Generate vertices
	UINT vertCount = m * n;

	const float dx = lengthX / (n - 1);
	const float dz = lengthZ / (m - 1);

	meshData.vertices.resize(vertCount);

	UINT iv = 0; // vertices index
	float z = halfZ;
	for (UINT iz = 0; iz < m; ++iz)
	{
		float x = -halfX;
		for (UINT ix = 0; ix < n; ++ix)
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

void GeometryGenerator::CreateCylinder(float bottomRadius, float topRadius, float height, UINT sliceCount, UINT stackCount, MeshData& meshData)
{
	meshData.vertices.clear();
	meshData.indices.clear();

	float stackHeight = height / stackCount;
	float bottomHeight = -0.5f * height;
	float topHeight = 0.5f * height;

	// Radius change in each ring moving up from bottom
	float ringRadiusStep = (topRadius - bottomRadius) / stackCount;

	// Compute vertices
	UINT ringCount = stackCount + 1;
	for (UINT ir = 0; ir < ringCount; ++ir)
	{
		float y = bottomHeight + ir * stackHeight;
		float r = bottomRadius + ir * ringRadiusStep;

		CreateRingVertex(r, y, sliceCount, meshData, false);
	}

	// Compute indices
	// Add one because we duplicate the first and last vertex per ring
	// since the texture coordinates are different betwwen the first and the last vertex.
	UINT ringVertexCount = sliceCount + 1;
	for (UINT iStack = 0; iStack < stackCount; ++iStack)
	{
		for (UINT iSlice = 0; iSlice < sliceCount; ++iSlice)
		{
			meshData.indices.push_back(iStack * ringVertexCount + iSlice);
			meshData.indices.push_back((iStack + 1) * ringVertexCount + iSlice);
			meshData.indices.push_back((iStack + 1) * ringVertexCount + iSlice + 1);

			meshData.indices.push_back((iStack + 1) * ringVertexCount + iSlice + 1);
			meshData.indices.push_back(iStack * ringVertexCount + iSlice + 1);
			meshData.indices.push_back(iStack * ringVertexCount + iSlice);
		}
	}

	// Create bottom cap and top cap
	CreateCylinderCap(bottomRadius, bottomHeight, sliceCount, meshData, false);
	CreateCylinderCap(topRadius, topHeight, sliceCount, meshData, true);

	return;
}

void GeometryGenerator::CreateCylinderCap(float radius, float y, UINT sliceCount, MeshData& meshData, bool faceUp)
{
	// Reserve index before concatnating vertices
	UINT baseIndex = meshData.vertices.size();

	CreateRingVertex(radius, y, sliceCount, meshData, faceUp);

	// Create center vertex
	Vertex centerVert;
	centerVert.position = XMFLOAT3(0, y, 0);
	meshData.vertices.push_back(centerVert);
	
	UINT centerIndex = meshData.vertices.size() - 1;

	// Calculate index
	for (UINT i = 0; i < sliceCount; ++i)
	{
		meshData.indices.push_back(centerIndex);
		meshData.indices.push_back(baseIndex + i);
		meshData.indices.push_back(baseIndex + i + 1);
	}
}

void GeometryGenerator::CreateRingVertex(float radius, float y, UINT sliceCount, MeshData& meshData, bool faceUp)
{
	// Theta step to rotate around
	float dTheta = 2.0f * XM_PI / sliceCount;
	if (faceUp)
		dTheta *= -1;

	// The start end the end vertices is overlapped because texCoord is different
	float theta = 0;
	for (UINT i = 0; i <= sliceCount; ++i)
	{
		Vertex vert;

		float x = radius * cosf(theta);
		float z = radius * sinf(theta);

		vert.position = XMFLOAT3(x, y, z);

		meshData.vertices.push_back(vert);

		theta += dTheta;
	}
}
