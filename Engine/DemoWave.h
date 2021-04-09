#pragma once
#include "NPGameEngine.h"
class DemoWave : public NPGameEngine
{
public:
	DemoWave(HINSTANCE hInstance);
	~DemoWave();

protected:
	virtual void BuildGeometryBuffers() override;

protected:
	BaseObject* mWaveMesh;
	BaseObject* mCenterObject;
	BaseObject* mGridObject;
};

