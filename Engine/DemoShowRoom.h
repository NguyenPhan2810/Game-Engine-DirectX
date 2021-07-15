#pragma once

#include "NPGameEngine.h"
#include "Texture.h"
#include <memory>

class DemoShowRoom : public NPGameEngine
{
public:
	DemoShowRoom(HINSTANCE hInstance);
	~DemoShowRoom();

	virtual bool Init() override;
	virtual void UpdateScene() override;
	virtual void DrawScene() override;

protected:
	void CreateObjects();
	void CreateTextures();
	void CreateMaterials();

protected:
	std::shared_ptr<Texture> mCrateTex;
	std::shared_ptr<Texture> mDarkBrickTex;
	std::shared_ptr<Texture> mCheckBoardTex;
	std::shared_ptr<Texture> mMirrorTex;

	std::shared_ptr<BaseObject> mMirrorObj;
	std::shared_ptr<BaseObject> mCenterObj;
	std::shared_ptr<BaseObject> mFloorObj;
	std::shared_ptr<BaseObject> mLWallObj;
	std::shared_ptr<BaseObject> mRWallObj;
};