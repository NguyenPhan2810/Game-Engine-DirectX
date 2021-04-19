#include "stdafx.h"
#include "BaseObject.h"
#include "Renderer.h"

UINT BaseObject::mIdBase = 0;
bool BaseObject::mAllObjectsChanged = false;
std::map<UINT, BaseObject*> BaseObject::mAllObjectsMap;
std::vector<BaseObject*> BaseObject::mAllObjectsVec;

BaseObject::BaseObject()
: transform(nullptr)
{
	mAllObjectsChanged = true;

	mId = mIdBase;
	mIdBase++;
	mAllObjectsMap[mId] = this;

	transform = new Transform();
	AddComponent(transform);
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