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
	virtual void UpdateScene() override;
	//virtual void DrawScene() override;

protected:
	void BuildGeometryBuffers();

protected:
	Waves mWaves;

	BaseObject* mWaveMesh;
	BaseObject* mCenterObject;
	BaseObject* mGridObject;
};

