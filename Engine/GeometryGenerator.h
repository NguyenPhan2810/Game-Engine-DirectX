#pragma once

#include "d3dUtil.h"
#include "Vertex.h"

using namespace Vertex;
class GeometryGenerator
{
public:
	struct Vertex
	{
		Vertex() {}
		Vertex(const XMFLOAT3& p, const XMFLOAT3& n, const XMFLOAT3& t, const XMFLOAT2& uv)
		: Position(p)
		, Normal(n)
		, TangentU(t)
		, TexC(uv)
		{}

		Vertex(
			float px, float py, float pz,
			float nx, float ny, float nz,
			float tx, float ty, float tz,
			float u, float v)
			: Position(px, py, pz)
			, Normal(nx, ny, nz)
			, TangentU(tx, ty, tz)
			, TexC(u, v) {}
		
		XMFLOAT3 Position;
		XMFLOAT3 Normal;
		XMFLOAT3 TangentU;
		XMFLOAT2 TexC;
	};

	struct MeshData
	{
		std::vector<Vertex> Vertices;
		std::vector<UINT> Indices;
	};

	///<summary>
	/// Copy position from source to Vertices
	///</summary>
	static void ConvertToBasic32(std::vector<GeometryGenerator::Vertex>& source, std::vector<Basic32>& Vertices);

	///<summary>
	/// Creates a mesh from simple file
	///</summary>
	static void CreateFromFile(const std::wstring& filepath, MeshData& meshData);

	///<summary>
	/// Creates a box centered at the origin with the given dimensions.
	///</summary>
	static void CreateBox(float width, float height, float depth, MeshData& meshData);

	///<summary>
	/// Creates a sphere centered at the origin with the given radius.  The
	/// slices and stacks parameters control the degree of tessellation.
	///</summary>
	void CreateSphere(float radius, UINT sliceCount, UINT stackCount, MeshData& meshData);

	///<summary>
	/// Creates a geosphere centered at the origin with the given radius.  The
	/// slices and stacks parameters control the degree of tessellation.
	///</summary>
	void CreateGeoSphere(float radius, UINT nSubdivisions, MeshData& meshData);

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
	static void CreateGrid(float lengthX, float lengthZ, UINT m, UINT n, MeshData& meshData);

	/// <summary>
	/// Create a cylinder parallel to y axis and centered about the origin
	/// The sliceCount and stackCount control level of tessellation
	/// </summary>
	void CreateCylinder(float bottomRadius, float topRadius, float height, UINT sliceCount,
		UINT stackCount, MeshData& meshData);

private:
	/// <summary>
	/// Subdivide a triangle into 4 sub-triangles
	/// </summary>
	void Subdivide(MeshData& meshData);

	/// <summary>
	/// Create a seperate cylinder cap Vertices that doesnt connect toe the cylinder it self
	/// faceUp set to true will create triangle be seen from top culling bottom
	/// </summary>
	void CreateCylinderCap(float radius, float y, UINT sliceCount, MeshData& meshData, bool faceUp);

};

