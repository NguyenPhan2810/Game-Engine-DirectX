#include "stdafx.h"
#include "Renderer.h"
#include "BaseObject.h"

Renderer::Renderer()
: mVertexBuffer(nullptr)
, mIndexBuffer(nullptr)
, mIndexCount(0)
, mVertexCount(0)
{
	name = "Renderer";
	// Default material
	mMaterial.ambient = XMFLOAT4(0.6f, 0.6f, 0.6f, 1.0f);
	mMaterial.diffuse = XMFLOAT4(0.6f, 0.6f, 0.6f, 1.0f);
	mMaterial.Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 16.0f);
}

Renderer::~Renderer()
{
	ReleaseCOM(mVertexBuffer);
	ReleaseCOM(mIndexBuffer);
}

void Renderer::Init()
{
}

void Renderer::Update(float dt)
{
}

void Renderer::Draw()
{
	auto context = attachedObject->GetImmediateContext();
	UINT stride = sizeof(Vertex::PosNormal);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, &mVertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	context->DrawIndexed(mIndexCount, 0, 0);
}

void Renderer::LoadGeometry(const GeometryGenerator::MeshData meshData)
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

void Renderer::CreateVertexBuffer(const std::vector<Vertex::PosNormal>& vertexData, D3D11_BUFFER_DESC vbd)
{
	ReleaseCOM(mVertexBuffer);

	D3D11_SUBRESOURCE_DATA vertexInitData;
	vertexInitData.pSysMem = vertexData.data();

	HR(attachedObject->GetDevice()->CreateBuffer(&vbd, &vertexInitData, &mVertexBuffer));
}

void Renderer::CreateIndexBuffer(const std::vector<UINT>& indexData, D3D11_BUFFER_DESC ibd)
{
	ReleaseCOM(mIndexBuffer);

	D3D11_SUBRESOURCE_DATA indexInitData;
	indexInitData.pSysMem = indexData.data();

	HR(attachedObject->GetDevice()->CreateBuffer(&ibd, &indexInitData, &mIndexBuffer));
}

ID3D11Buffer* Renderer::GetVertexBuffer()
{
	return mVertexBuffer;
}

UINT Renderer::GetVertexCount() const
{
	return mVertexCount;
}

ID3D11Buffer* Renderer::GetIndexBuffer()
{
	return mIndexBuffer;
}

UINT Renderer::GetIndexCount() const
{
	return mIndexCount;
}

Material& Renderer::GetMaterial()
{
	return mMaterial;
}
