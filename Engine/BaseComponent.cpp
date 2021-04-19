#include "stdafx.h"
#include "BaseComponent.h"
#include "BaseObject.h"

BaseComponent::BaseComponent()
: name("BaseComponent")
{

}

BaseComponent::~BaseComponent()
{
}

void BaseComponent::Init()
{
}

void BaseComponent::Update(float dt)
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
