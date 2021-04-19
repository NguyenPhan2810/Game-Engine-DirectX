#pragma once

#include "NPInterfaces.h"
#include <string>

class BaseObject;

class BaseComponent : public ILifeCycle
{
public:
	BaseComponent();
	virtual ~BaseComponent();

	virtual void Init() override;
	virtual void Update(float dt = 0) override;
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

