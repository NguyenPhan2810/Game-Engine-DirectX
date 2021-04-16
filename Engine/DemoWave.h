#pragma once
#include "NPGameEngine.h"
#include "Waves.h"
#include "MathHelper.h"
#include "LightHelper.h"

class DemoWave : public NPGameEngine
{
public:
	DemoWave(HINSTANCE hInstance);
	~DemoWave();

	virtual bool Init() override;
	virtual void UpdateScene(float dt = 0) override;
	//virtual void DrawScene() override;

protected:
	virtual void BuildGeometryBuffers() override;

protected:
	Waves mWaves;

	BaseObject* mWaveMesh;
	BaseObject* mCenterObject;
	BaseObject* mGridObject;
};

