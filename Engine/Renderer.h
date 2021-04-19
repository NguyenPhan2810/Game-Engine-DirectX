#pragma once

#include "BaseComponent.h"
#include "d3dUtil.h"
#include "GeometryGenerator.h"
#include "LightHelper.h"
#include "Texture.h"
#include <memory>

/// <summary>
/// Convinent macro for get this component
/// </summary>
#define RENDERER(x) x->GetComponent<Renderer>("Renderer")

class Renderer : public BaseComponent
{
public:
	Renderer();
	~Renderer() override;

	virtual void Init() override;
	virtual void Update() override;
	virtual void Draw() override;

private:
	Renderer(const Renderer& renderer) {}
	void operator=(const Renderer& renderer) {}

public:
	void LoadGeometry(const GeometryGenerator::MeshData& meshData);
	void CreateVertexBuffer(const std::vector<Vertex::Basic32>& vertexData, D3D11_BUFFER_DESC vbd);
	void CreateIndexBuffer(const std::vector<UINT>& indexData, D3D11_BUFFER_DESC ibd);

	ID3D11Buffer* GetVertexBuffer();
	UINT GetVertexCount() const;
	ID3D11Buffer* GetIndexBuffer();
	UINT GetIndexCount() const;

	Material& GetMaterial();

	std::shared_ptr<Texture> Texture;

protected:
	ID3D11Buffer* mVertexBuffer;
	ID3D11Buffer* mIndexBuffer;

	UINT mVertexCount;
	UINT mIndexCount;

	Material mMaterial;
};
