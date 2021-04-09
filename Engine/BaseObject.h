#pragma once

#include "d3dUtil.h"
#include "GeometryGenerator.h"
#include "GlobalDefinitions.h"

class BaseObject
{
public:
	BaseObject(ID3D11Device* device, ID3D11DeviceContext* immediateContext);
	~BaseObject();

	static std::vector<BaseObject*> GetAllObjects();

	virtual void Update(float dt);
	virtual void Draw();
	
public:
	void LoadGeometry(const GeometryGenerator::MeshData meshData);
	void CreateVertexBuffer(const std::vector<GLOBDEF::Vertex>& vertexData, D3D11_BUFFER_DESC vbd);
	void CreateIndexBuffer(const std::vector<UINT>& indexData, D3D11_BUFFER_DESC ibd);

	ID3D11Buffer* GetVertexBuffer();
	UINT GetVertexCount() const;
	ID3D11Buffer* GetIndexBuffer();
	UINT GetIndexCount() const;

	void Translate(const XMFLOAT3& displacement);
	void Rorate(const XMFLOAT3& rotationOrigin, float radian);
	void Scale(const XMFLOAT3& scaleElements);

	XMMATRIX LocalToWorldMatrix() const;

public:
	bool renderWireframe;

protected:
	ID3D11Device* mDevice;
	ID3D11DeviceContext* mImmediateContext;

	ID3D11Buffer* mVertexBuffer;
	ID3D11Buffer* mIndexBuffer;

	UINT mVertexCount;
	UINT mIndexCount;

	XMMATRIX mWorldMatrix;

private:
	UINT mId;
	static UINT mIdBase;
	static std::map<UINT, BaseObject*> mAllObjectsMap; // Map ID to object's pointer

	static bool mAllObjectsChanged;
	static std::vector<BaseObject*> mAllObjectsVec; // Convience way to get all objects outside the class
};

