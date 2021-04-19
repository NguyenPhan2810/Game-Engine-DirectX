#include "stdafx.h"
#include "BaseObject.h"
#include "Renderer.h"

UINT BaseObject::mIdBase = 0;
bool BaseObject::mAllObjectsChanged = false;
std::map<UINT, BaseObject*> BaseObject::mAllObjectsMap;
std::vector<BaseObject*> BaseObject::mAllObjectsVec;

BaseObject::BaseObject(ID3D11Device* device, ID3D11DeviceContext* immediateContext)
: mDevice(device)
, mImmediateContext(immediateContext)
{
	mAllObjectsChanged = true;

	mId = mIdBase;
	mIdBase++;
	mAllObjectsMap[mId] = this;

	mWorldMatrix = XMMatrixIdentity();

	AddComponent(new Renderer());
}

BaseObject::~BaseObject()
{
	for (auto& comp : mComponents)
		SafeDelete(comp);

	mAllObjectsChanged = true;
	mAllObjectsMap.erase(mId);
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

void BaseObject::Init()
{
	for (auto comp : mComponents)
		comp->Init();
}

void BaseObject::Update(float dt)
{
	for (auto comp : mComponents)
		comp->Update(dt);
}

void BaseObject::Draw()
{
	for (auto comp : mComponents)
		comp->Draw();
}

void BaseObject::AddComponent(BaseComponent* comp)
{
	comp->attach(this);
	mComponents.push_back(comp);
}

void BaseObject::RemoveComponent(const std::string& name)
{
	for (auto it = mComponents.begin(); it != mComponents.end(); ++it)
		if ((*it)->name == name)
		{
			mComponents.erase(it);
			break;
		}
}

BaseComponent* BaseObject::GetComponentByName(const std::string& name)
{
	for (auto comp : mComponents)
		if (comp->name == name)
			return comp;

	return nullptr;
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