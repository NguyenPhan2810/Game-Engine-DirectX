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
	void CreateObjects();
	void CreateTextures();
	void CreateMaterials();

protected:
	std::shared_ptr<Texture> mCrateTex;
	std::vector<std::shared_ptr<Texture>> mFireTex;
	UINT mFireTexIndex;
	std::shared_ptr<Texture> mDarkBrickTex;
	std::shared_ptr<Texture> mCylTex;
	std::shared_ptr<Texture> mSphereTex;

	std::shared_ptr<BaseObject> mCrateObject;
	std::shared_ptr<BaseObject> mFloorObject;
	std::shared_ptr<BaseObject> mCenterBox;
	std::vector<std::shared_ptr<BaseObject>> mCylinders;
	std::vector<std::shared_ptr<BaseObject>> mSpheres;
};

