
#include "BaseComponent.h"
#include "BaseObject.h"

BaseComponent::BaseComponent()
: name("BaseComponent")
, attachedObject(nullptr)
{

}

BaseComponent::~BaseComponent()
{
}

void BaseComponent::Init()
{
}

void BaseComponent::Update()
{
}

void BaseComponent::FixedUpdate()
{
}

void BaseComponent::Draw()
{
}

void BaseComponent::attach(BaseObject* obj)
{
	detach();
	attachedObject = obj;
}

void BaseComponent::detach()
{
	if (attachedObject)
		attachedObject->RemoveComponent(name);
	attachedObject = nullptr;
}
