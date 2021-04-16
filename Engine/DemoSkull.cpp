#include "DemoSkull.h"

DemoSkull::DemoSkull(HINSTANCE hInstance)
: NPGameEngine(hInstance)
{
	mCamRadius = 10;
	mDirLight.ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mDirLight.diffuse = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mDirLight.specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mDirLight.direction = XMFLOAT3(0.57735f, -0.57735f, 0.57735f);
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
	mGridObject->LoadGeometry(grid);

	mCenterObject = new BaseObject(mDevice, mImmediateContext);
	mCenterObject->Translate(XMFLOAT3(0, 1, 0));
	mCenterObject->Scale(XMFLOAT3(0.7, 0.7, 0.7));
	mCenterObject->LoadGeometry(skull);

	mCenterBox = new BaseObject(mDevice, mImmediateContext);
	mCenterBox->Translate(XMFLOAT3(0, 0.5, 0));
	mCenterBox->Scale(XMFLOAT3(2, 1, 2));
	mCenterBox->LoadGeometry(box);

	for (int i = 0; i < 5; ++i)
	{
		auto cylinder1 = new BaseObject(mDevice, mImmediateContext);
		cylinder1->Translate(XMFLOAT3(-5.0f, 1.5f, -10.0f + i * 5.0f));
		cylinder1->LoadGeometry(cylinder);
		auto cylinder2 = new BaseObject(mDevice, mImmediateContext);
		cylinder2->Translate(XMFLOAT3(+5.0f, 1.5f, -10.0f + i * 5.0f));
		cylinder2->LoadGeometry(cylinder);

		mCylinders.push_back(cylinder1);
		mCylinders.push_back(cylinder2);

		auto sphere1 = new BaseObject(mDevice, mImmediateContext);
		sphere1->Translate(XMFLOAT3(-5.0f, 3.5f, -10.0f + i * 5.0f));
		sphere1->LoadGeometry(sphere);
		auto sphere2 = new BaseObject(mDevice, mImmediateContext);
		sphere2->Translate(XMFLOAT3(+5.0f, 3.5f, -10.0f + i * 5.0f));
		sphere2->LoadGeometry(sphere);

		mSpheres.push_back(sphere1);
		mSpheres.push_back(sphere2);
	}
}
