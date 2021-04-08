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
	/// Generate a grid from top-left to bot-right lies in XZ plane
	///  with m rows and n column
	///       Z ^            
	///         |            
	///         |       X    
	/// ---------------->   
	///         |            
	///         |            
	///         |      
	/// </summary>
	void CreateGrid(float lengthX, float lengthZ, UINT m, UINT n, MeshData& meshData);

	/// <summary>
	/// Create a cylinder parallel to y axis and centered about the origin
	/// The sliceCount and stackCount control level of tessellation
	/// </summary>
	void CreateCylinder(float bottomRadius, float topRadius, float height, UINT sliceCount,
		UINT stackCount, MeshData& meshData);

private:
	/// <summary>
	/// Create a seperate cylinder cap vertices that doesnt connect toe the cylinder it self
	/// faceUp set to true will create triangle be seen from top culling bottom
	/// </summary>
	void CreateCylinderCap(float radius, float y, UINT sliceCount, MeshData& meshData, bool faceUp);
};

