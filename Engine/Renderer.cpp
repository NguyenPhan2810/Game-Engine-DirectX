#include "stdafx.h"
#include "Renderer.h"
#include "BaseObject.h"
#include "d3dApp.h"

Renderer::Renderer()
: mVertexBuffer(nullptr)
, mIndexBuffer(nullptr)
, mIndexCount(0)
, mVertexCount(0)
, Texture(nullptr)
{
	name = "Renderer";
	// Default material
	mMaterial.Ambient = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	mMaterial.Diffuse = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mMaterial.Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 16.0f);

	// Default texture transform
	XMMATRIX I = XMMatrixIdentity();
	XMStoreFloat4x4(&TexTransform, I);
}

Renderer::~Renderer()
{
	ReleaseCOM(mVertexBuffer);
	ReleaseCOM(mIndexBuffer);
}

void Renderer::Init()
{
}

void Renderer::Update()
{
}

void Renderer::Draw()
{
	auto context = D3DApp::GetImmediateContext();
	UINT stride = sizeof(Vertex::Basic32);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, &mVertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	context->DrawIndexed(mIndexCount, 0, 0);
}

void Renderer::LoadGeometry(const GeometryGenerator::MeshData& meshData)
{
	mVertexCount = meshData.Vertices.size();

	std::vector<Vertex::Basic32> Vertices;
	for (UINT i = 0; i < mVertexCount; ++i)
	{
		Vertices.push_back(Vertex::Basic32
			{
				meshData.Vertices[i].Position,
				meshData.Vertices[i].Normal,
				meshData.Vertices[i].TexC
			});
	}

	// Create vertex buffer
	D3D11_BUFFER_DESC vbd{ 0 };
	vbd.ByteWidth = mVertexCount * sizeof(Vertex::Basic32);
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	CreateVertexBuffer(Vertices, vbd);

	// Create index buffer
	mIndexCount = meshData.Indices.size();

	D3D11_BUFFER_DESC ibd{ 0 };
	ibd.ByteWidth = mIndexCount * sizeof(UINT);
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;

	CreateIndexBuffer(meshData.Indices, ibd);
}

void Renderer::CreateVertexBuffer(const std::vector<Vertex::Basic32>& vertexData, D3D11_BUFFER_DESC vbd)
{
	ReleaseCOM(mVertexBuffer);

	D3D11_SUBRESOURCE_DATA vertexInitData;
	vertexInitData.pSysMem = vertexData.data();

	HR(D3DApp::GetDevice()->CreateBuffer(&vbd, &vertexInitData, &mVertexBuffer));
}

void Renderer::CreateIndexBuffer(const std::vector<UINT>& indexData, D3D11_BUFFER_DESC ibd)
{
	ReleaseCOM(mIndexBuffer);

	D3D11_SUBRESOURCE_DATA indexInitData;
	indexInitData.pSysMem = indexData.data();

	HR(D3DApp::GetDevice()->CreateBuffer(&ibd, &indexInitData, &mIndexBuffer));
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
