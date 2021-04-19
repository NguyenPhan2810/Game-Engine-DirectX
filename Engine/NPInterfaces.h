#pragma once

// All class use NPGameEngine life cycle must implements this
class ILifeCycle
{
public:
	virtual void Init() = 0;
	virtual void Update(float dt = 0) = 0;
	virtual void Draw() = 0;
};