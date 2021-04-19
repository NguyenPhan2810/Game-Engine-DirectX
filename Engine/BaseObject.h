#pragma once

#include "d3dUtil.h"
#include "GeometryGenerator.h"
#include "GlobalDefinitions.h"
#include "LightHelper.h"
#include "BaseComponent.h"
#include "Transform.h"

class BaseObject : public ILifeCycle
{
public:
	BaseObject();
	~BaseObject();

	static std::vector<BaseObject*> GetAllObjects();

	virtual void Init() override;
	virtual void Update(float dt = 0) override;
	virtual void Draw() override;

public:
	void AddComponent(BaseComponent* comp);
	void RemoveComponent(const std::string& name);
	BaseComponent* GetComponentByName(const std::string& name);

	template <class _Ty>
	_Ty* GetComponent(const std::string& name);

public:
	// Because every object must have a transform component
	// So this comp deserves its own place
	Transform* transform;

private:
	UINT mId;
	static UINT mIdBase;
	static std::map<UINT, BaseObject*> mAllObjectsMap; // Map ID to object's pointer

	static bool mAllObjectsChanged;
	static std::vector<BaseObject*> mAllObjectsVec; // Convience way to get all objects outside the class

	std::vector< BaseComponent*> mComponents;
};

template <class _Ty>
inline _Ty* BaseObject::GetComponent(const std::string& name)
{
	return reinterpret_cast<_Ty*>(GetComponentByName(name));
}