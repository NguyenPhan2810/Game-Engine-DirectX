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
	void BuildGeometryBuffers();

protected:
	std::shared_ptr<Texture> mCrateTex;
	std::vector<std::shared_ptr<Texture>> mFireTex;
	UINT mFireTexIndex;
	std::shared_ptr<Texture> mDarkBrickTex;
	std::shared_ptr<Texture> mCylTex;
	std::shared_ptr<Texture> mSphereTex;

	std::shared_ptr<BaseObject> mCenterObject;
	std::shared_ptr<BaseObject> mGridObject;
	std::shared_ptr<BaseObject> mCenterBox;
	std::vector<std::shared_ptr<BaseObject>> mCylinders;
	std::vector<std::shared_ptr<BaseObject>> mSpheres;

};
