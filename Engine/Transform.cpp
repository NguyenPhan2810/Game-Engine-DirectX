#include "stdafx.h"
#include "Transform.h"

Transform::Transform()
{
	mWorldMatrix = XMMatrixIdentity();
}

Transform::~Transform()
{

}

void Transform::Init()
{

}

void Transform::Update()
{

}

void Transform::Draw()
{
}

void Transform::Translate(const XMFLOAT3& displacement)
{
	XMMATRIX translateMatrix = XMMatrixTranslation(displacement.x, displacement.y, displacement.z);
	mWorldMatrix = XMMatrixMultiply(mWorldMatrix, translateMatrix);
}

void Transform::Rotate(const XMFLOAT3& rotationOrigin, float radian)
{
	XMMATRIX rotateMatrix = XMMatrixRotationAxis(XMLoadFloat3(&rotationOrigin), radian);
	mWorldMatrix = XMMatrixMultiply(mWorldMatrix, rotateMatrix);
}

void Transform::Scale(const XMFLOAT3& scaleElements)
{
	XMMATRIX scaleMatrix = XMMatrixScaling(scaleElements.x, scaleElements.y, scaleElements.z);
	mWorldMatrix = XMMatrixMultiply(mWorldMatrix, scaleMatrix);
}

XMMATRIX Transform::LocalToWorldMatrix() const
{
	return mWorldMatrix;
}
