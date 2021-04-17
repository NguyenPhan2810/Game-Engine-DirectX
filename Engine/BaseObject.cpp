#include "stdafx.h"
#include "BaseObject.h"


UINT BaseObject::mIdBase = 0;
bool BaseObject::mAllObjectsChanged = false;
std::map<UINT, BaseObject*> BaseObject::mAllObjectsMap;
std::vector<BaseObject*> BaseObject::mAllObjectsVec;

BaseObject::BaseObject(ID3D11Device* device, ID3D11DeviceContext* immediateContext)
: mVertexBuffer(nullptr)
, mIndexBuffer(nullptr)
, mIndexCount(0)
, mVertexCount(0)
, mDevice(device)
, mImmediateContext(immediateContext)
, renderWireframe(false)
{
	mAllObjectsChanged = true;

	mId = mIdBase;
	mIdBase++;
	mAllObjectsMap[mId] = this;

	mWorldMatrix = XMMatrixIdentity();

	// Default material
	mMaterial.ambient = XMFLOAT4(0.6f, 0.6f, 0.6f, 1.0f);
	mMaterial.diffuse = XMFLOAT4(0.6f, 0.6f, 0.6f, 1.0f);
	mMaterial.specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 16.0f);
}

BaseObject::~BaseObject()
{
	mAllObjectsChanged = true;
	mAllObjectsMap.erase(mId);

	ReleaseCOM(mVertexBuffer);
	ReleaseCOM(mIndexBuffer);
}

std::vector<BaseObject*>  BaseObject::GetAllObjects()
{
	if (mAllObjectsChanged)
	{
		// Remake the vector
		mAllObjectsVec.clear();
		for (const auto& obj : mAllObjectsMap)
			mAllObjectsVec.push_back(obj.second);
	}

	return mAllObjectsVec;
}

void BaseObject::Update(float dt)
{

}

void BaseObject::Draw()
{
	UINT stride = sizeof(Vertex::PosNormal);
	UINT offset = 0;
	mImmediateContext->IASetVertexBuffers(0, 1, &mVertexBuffer, &stride, &offset);
	mImmediateContext->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	mImmediateContext->DrawIndexed(mIndexCount, 0, 0);
}

void BaseObject::LoadGeometry(const GeometryGenerator::MeshData meshData)
{
	mVertexCount = meshData.vertices.size();

	std::vector<Vertex::PosNormal> vertices;
	for (UINT i = 0; i < mVertexCount; ++i)
	{
		vertices.push_back(Vertex::PosNormal{ meshData.vertices[i].position, meshData.vertices[i].normal });
	}

	// Create vertex buffer
	D3D11_BUFFER_DESC vbd{ 0 };
	vbd.ByteWidth = mVertexCount * sizeof(Vertex::PosNormal);
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	CreateVertexBuffer(vertices, vbd);

	// Create index buffer
	mIndexCount = meshData.indices.size();

	D3D11_BUFFER_DESC ibd{ 0 };
	ibd.ByteWidth = mIndexCount * sizeof(UINT);
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;

	CreateIndexBuffer(meshData.indices, ibd);
}

void BaseObject::CreateVertexBuffer(const std::vector<Vertex::PosNormal>& vertexData, D3D11_BUFFER_DESC vbd)
{
	ReleaseCOM(mVertexBuffer);
	
	D3D11_SUBRESOURCE_DATA vertexInitData;
	vertexInitData.pSysMem = vertexData.data();

	HR(mDevice->CreateBuffer(&vbd, &vertexInitData, &mVertexBuffer));
}

void BaseObject::CreateIndexBuffer(const std::vector<UINT>& indexData, D3D11_BUFFER_DESC ibd)
{
	ReleaseCOM(mIndexBuffer);

	D3D11_SUBRESOURCE_DATA indexInitData;
	indexInitData.pSysMem = indexData.data();

	HR(mDevice->CreateBuffer(&ibd, &indexInitData, &mIndexBuffer));
}

ID3D11Buffer* BaseObject::GetVertexBuffer()
{
	return mVertexBuffer;
}

UINT BaseObject::GetVertexCount() const
{
	return mVertexCount;
}

ID3D11Buffer* BaseObject::GetIndexBuffer()
{
	return mIndexBuffer;
}

UINT BaseObject::GetIndexCount() const
{
	return mIndexCount;
}

void BaseObject::Translate(const XMFLOAT3& displacement)
{
	XMMATRIX translateMatrix = XMMatrixTranslation(displacement.x, displacement.y, displacement.z);
	mWorldMatrix = XMMatrixMultiply(translateMatrix, mWorldMatrix);
}

void BaseObject::Rorate(const XMFLOAT3& rotationOrigin, float radian)
{
	XMMATRIX rotateMatrix = XMMatrixRotationAxis(XMLoadFloat3(&rotationOrigin), radian);
	mWorldMatrix = XMMatrixMultiply(rotateMatrix, mWorldMatrix);
}

void BaseObject::Scale(const XMFLOAT3& scaleElements)
{
	XMMATRIX scaleMatrix = XMMatrixScaling(scaleElements.x, scaleElements.y, scaleElements.z);
	mWorldMatrix = XMMatrixMultiply(scaleMatrix, mWorldMatrix);
}

XMMATRIX BaseObject::LocalToWorldMatrix() const
{
	return mWorldMatrix;
}

Material& BaseObject::GetMaterial()
{
	return mMaterial;
}
