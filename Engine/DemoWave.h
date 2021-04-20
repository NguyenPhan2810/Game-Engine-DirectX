#pragma once
#include "NPGameEngine.h"
#include "Waves.h"
#include "MathHelper.h"
#include "LightHelper.h"
#include "Texture.h"

class DemoWave : public NPGameEngine
{
public:
	DemoWave(HINSTANCE hInstance);
	~DemoWave();

	virtual bool Init() override;
	virtual void UpdateScene() override;
	virtual void FixedUpdateScene() override;
	//virtual void DrawScene() override;

protected:
	void BuildGeometryBuffers();

protected:
	Waves mWaves;

	std::shared_ptr<BaseObject> mWaveMesh;
	std::shared_ptr<BaseObject> mCenterObject;
	std::shared_ptr<BaseObject> mGridObject;
	std::shared_ptr<Texture> mLandTex;
	std::shared_ptr<Texture> mWaveTex;

	XMFLOAT2 mWaveTexOffset;
};

