#pragma once
#include "NPGameEngine.h"
#include "Texture.h"
#include <memory>

class DemoShape : public NPGameEngine
{
public:
	DemoShape(HINSTANCE hInstance);
	~DemoShape();

	virtual bool Init() override;
	virtual void UpdateScene() override;
protected:
	virtual void BuildGeometryBuffers() override;

protected:
	std::shared_ptr<Texture> mCrateTex;

	BaseObject* mCenterObject;
	BaseObject* mGridObject;
	BaseObject* mCenterBox;
	std::vector<BaseObject*> mCylinders;
	std::vector<BaseObject*> mSpheres;
};

