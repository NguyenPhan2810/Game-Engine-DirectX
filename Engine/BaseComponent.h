#pragma once


#include "NPInterfaces.h"

class BaseObject;

class BaseComponent : public ILifeCycle
{
public:
	BaseComponent();
	virtual ~BaseComponent();

	virtual void Init() override;
	virtual void Update() override;
	virtual void FixedUpdate() override;
	virtual void Draw() override;

	std::string name;

public:
	// These should not be called
	// Use BaseObject::AddComponent and BaseObject::RemoveComponet instead
	void attach(BaseObject* obj);
	void detach();

protected:
	BaseObject* attachedObject;
};

