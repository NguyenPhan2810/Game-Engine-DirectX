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

void Transform::Update(float dt)
{

}

void Transform::Draw()
{
}

void Transform::Translate(const XMFLOAT3& displacement)
{
	XMMATRIX translateMatrix = XMMatrixTranslation(displacement.x, displacement.y, displacement.z);
	mWorldMatrix = XMMatrixMultiply(translateMatrix, mWorldMatrix);
}

void Transform::Rorate(const XMFLOAT3& rotationOrigin, float radian)
{
	XMMATRIX rotateMatrix = XMMatrixRotationAxis(XMLoadFloat3(&rotationOrigin), radian);
	mWorldMatrix = XMMatrixMultiply(rotateMatrix, mWorldMatrix);
}

void Transform::Scale(const XMFLOAT3& scaleElements)
{
	XMMATRIX scaleMatrix = XMMatrixScaling(scaleElements.x, scaleElements.y, scaleElements.z);
	mWorldMatrix = XMMatrixMultiply(scaleMatrix, mWorldMatrix);
}

XMMATRIX Transform::LocalToWorldMatrix() const
{
	return mWorldMatrix;
}
