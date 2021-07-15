
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

XMFLOAT3 Transform::GetWorldPosition() const
{
	XMVECTOR O = { 0 };
	XMFLOAT3 result;

	XMStoreFloat3(&result, XMVector3Transform(O, mWorldMatrix));

	return result;
}

XMFLOAT3 Transform::GetWorldY() const
{
	
	XMVECTOR Y = XMVectorSet(0, 1, 0, 0);
	XMFLOAT3 result;
	
	XMFLOAT3 posFloat = GetWorldPosition();
	XMVECTOR pos = XMLoadFloat3(&posFloat);

	XMStoreFloat3(&result, XMVector3Normalize(XMVector3Transform(Y, mWorldMatrix) - pos));

	return result;
}

XMMATRIX Transform::LocalToWorldMatrix() const
{
	return mWorldMatrix;
}
