#pragma once

#include "d3dUtil.h"

class GeometryGenerator
{
public:
	struct Vertex
	{
		Vertex() {}
		Vertex(const XMFLOAT3& p, const XMFLOAT3& n, const XMFLOAT3& t, const XMFLOAT2& uv)
		: position(p)
		, normal(n)
		, tangentU(t)
		, texC(uv)
		{}

		Vertex(
			float px, float py, float pz,
			float nx, float ny, float nz,
			float tx, float ty, float tz,
			float u, float v)
			: position(px, py, pz)
			, normal(nx, ny, nz)
			, tangentU(tx, ty, tz)
			, texC(u, v) {}
		
		XMFLOAT3 position;
		XMFLOAT3 normal;
		XMFLOAT3 tangentU;
		XMFLOAT2 texC;
	};

	struct MeshData
	{
		std::vector<Vertex> vertices;
		std::vector<UINT> indices;
	};

	/// <summary>
	/// Generate a grid lies in XZ plane
	/// </summary>
	/// <param name="lengthX"> grid's length in X axis </param>
	/// <param name="lengthZ"> grid's length in Z axis </param>
	/// <param name="m"> number of vertices divided in Z direction </param>
	/// <param name="n"> number of vertices divided in X direction </param>
	/// <param name="meshData"> mesh generated </param>
	void CreateGrid(float lengthX, float lengthZ, UINT m, UINT n, MeshData& meshData);
};

