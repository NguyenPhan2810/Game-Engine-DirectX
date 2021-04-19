#pragma once
#include "NPGameEngine.h"
class DemoSkull : public NPGameEngine
{
public:
	DemoSkull(HINSTANCE hInstance);
	~DemoSkull();

	virtual void UpdateScene() override;
protected:
	virtual void BuildGeometryBuffers() override;

protected:
	BaseObject* mCenterObject;
	BaseObject* mGridObject;
	BaseObject* mCenterBox;
	std::vector<BaseObject*> mCylinders;
	std::vector<BaseObject*> mSpheres;
};

