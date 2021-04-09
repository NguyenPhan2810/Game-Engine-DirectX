#include "BaseObject.h"

BaseObject::BaseObject(ID3D11Device* device, ID3D11DeviceContext* immediateContext)
: mVertexBuffer(nullptr)
, mIndexBuffer(nullptr)
, mIndexCount(0)
, mVertexCount(0)
, mDevice(device)
, mImmediateContext(immediateContext)
{
	mWorldMatrix = XMMatrixIdentity();
}

BaseObject::~BaseObject()
{
	ReleaseCOM(mVertexBuffer);
	ReleaseCOM(mIndexBuffer);
}

void BaseObject::Update(float dt)
{
}

void BaseObject::Draw()
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	mImmediateContext->IASetVertexBuffers(0, 1, &mVertexBuffer, &stride, &offset);
	mImmediateContext->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	mImmediateContext->DrawIndexed(mIndexCount, 0, 0);
}

void BaseObject::LoadGeometry(const GeometryGenerator::MeshData meshData)
{
	mVertexCount = meshData.vertices.size();

	XMFLOAT4 black(0.0f, 0.0f, 0.0f, 1.0f);

	std::vector<Vertex> vertices;
	for (UINT i = 0; i < mVertexCount; ++i)
	{
		vertices.push_back(Vertex{ meshData.vertices[i].position, black });
	}

	// Create vertex buffer

	D3D11_BUFFER_DESC vbd;
	vbd.ByteWidth = mVertexCount * sizeof(Vertex);
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA vertexInitData;
	vertexInitData.pSysMem = vertices.data();

	HR(mDevice->CreateBuffer(&vbd, &vertexInitData, &mVertexBuffer));

	// Create index buffer
	mIndexCount = meshData.indices.size();

	D3D11_BUFFER_DESC ibd;
	ibd.ByteWidth = mIndexCount * sizeof(UINT);
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA indexInitData;
	indexInitData.pSysMem = meshData.indices.data();

	HR(mDevice->CreateBuffer(&ibd, &indexInitData, &mIndexBuffer));
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
