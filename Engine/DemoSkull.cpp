#include "stdafx.h"
#include "DemoSkull.h"
#include "Renderer.h"

DemoSkull::DemoSkull(HINSTANCE hInstance)
: NPGameEngine(hInstance)
{
	mCamRadius = 10;

	mDirLight.Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	mDirLight.Diffuse = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mDirLight.Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mDirLight.Direction = XMFLOAT3(0.57735f, -0.57735f, 0.57735f);

	mPointLight.Ambient = XMFLOAT4(0.1f, 0.1f, 0.17f, 1.0f);
	mPointLight.Diffuse = XMFLOAT4(0.7f, 0.7f, 0.9f, 1.0f);
	mPointLight.Specular = XMFLOAT4(0.7f, 0.7f, 0.9f, 1.0f);
	mPointLight.att = XMFLOAT3(1.0f, 0.1f, 0.5f);
	mPointLight.range = 30.0f;

	mSpotLight.Ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mSpotLight.Diffuse = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mSpotLight.Specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	mSpotLight.Position = XMFLOAT3(0, 5, 0);
	mSpotLight.Range = 20;
	mSpotLight.Direction = XMFLOAT3(0, -1, 0);
	mSpotLight.Att = XMFLOAT3(0, 1, 1);
}

DemoSkull::~DemoSkull()
{
	delete mCenterObject;
	delete mGridObject;
	delete mCenterBox;
	for (auto p : mCylinders)
		delete p;
	mCylinders.clear();
	for (auto p : mSpheres)
		delete p;
	mSpheres.clear();
}

void DemoSkull::UpdateScene(float dt)
{
	NPGameEngine::UpdateScene(dt);

	mPointLight.Position.x = 3.0f * cosf(2 * mTimer.TotalTime());
	mPointLight.Position.z = 3.0f * sinf(2 * mTimer.TotalTime());
	mPointLight.Position.y = 2;
}

void DemoSkull::BuildGeometryBuffers()
{
	GeometryGenerator::MeshData box;
	GeometryGenerator::MeshData grid;
	GeometryGenerator::MeshData sphere;
	GeometryGenerator::MeshData geoSphere;
	GeometryGenerator::MeshData cylinder;
	GeometryGenerator::MeshData skull;

	GeometryGenerator geoGen;
	geoGen.CreateBox(1.0f, 1.0f, 1.0f, box);
	geoGen.CreateGrid(20.0f, 30.0f, 60, 40, grid);
	geoGen.CreateGeoSphere(0.5, 2, geoSphere);
	geoGen.CreateSphere(0.5f, 10, 10, sphere);
	geoGen.CreateCylinder(1, 0.3f, 3.0f, 20, 20, cylinder);
	geoGen.CreateFromFile(L"Models/skull.txt", skull);
	mGridObject = new BaseObject(mDevice, mImmediateContext);
	RENDERER(mGridObject)->LoadGeometry(grid);

	mCenterObject = new BaseObject(mDevice, mImmediateContext);
	mCenterObject->Translate(XMFLOAT3(0, 1, 0));
	mCenterObject->Scale(XMFLOAT3(0.3, 0.3, 0.3));
	RENDERER(mCenterObject)->LoadGeometry(skull);

	mCenterBox = new BaseObject(mDevice, mImmediateContext);
	mCenterBox->Translate(XMFLOAT3(0, 0.5, 0));
	mCenterBox->Scale(XMFLOAT3(2, 1, 2));
	RENDERER(mCenterBox)->LoadGeometry(box);

	for (int i = 0; i < 5; ++i)
	{
		auto cylinder1 = new BaseObject(mDevice, mImmediateContext);
		cylinder1->Translate(XMFLOAT3(-5.0f, 1.5f, -10.0f + i * 5.0f));
		RENDERER(cylinder1)->LoadGeometry(cylinder);
		auto cylinder2 = new BaseObject(mDevice, mImmediateContext);
		cylinder2->Translate(XMFLOAT3(+5.0f, 1.5f, -10.0f + i * 5.0f));
		RENDERER(cylinder2)->LoadGeometry(cylinder);

		mCylinders.push_back(cylinder1);
		mCylinders.push_back(cylinder2);

		auto sphere1 = new BaseObject(mDevice, mImmediateContext);
		sphere1->Translate(XMFLOAT3(-5.0f, 3.5f, -10.0f + i * 5.0f));
		RENDERER(sphere1)->LoadGeometry(sphere);
		auto sphere2 = new BaseObject(mDevice, mImmediateContext);
		sphere2->Translate(XMFLOAT3(+5.0f, 3.5f, -10.0f + i * 5.0f));
		RENDERER(sphere2)->LoadGeometry(geoSphere);

		mSpheres.push_back(sphere1);
		mSpheres.push_back(sphere2);
	}	
	
	// Build mat
	auto& landMat = RENDERER(mGridObject)->GetMaterial();
	landMat.ambient = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
	landMat.diffuse = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
	landMat.Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 16.0f);

	auto& cubeMat = RENDERER(mCenterObject)->GetMaterial();
	cubeMat.ambient = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	cubeMat.diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	cubeMat.Specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 16.0f);
}
