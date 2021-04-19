#include "stdafx.h"
#include "DemoSkull.h"
#include "Renderer.h"
#include "Cube.h"

DemoSkull::DemoSkull(HINSTANCE hInstance)
: NPGameEngine(hInstance)
{
	mCamRadius = 10;
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

}

void DemoSkull::BuildGeometryBuffers()
{
	GeometryGenerator::MeshData sphere;
	GeometryGenerator::MeshData geoSphere;
	GeometryGenerator::MeshData cylinder;
	GeometryGenerator::MeshData skull;

	GeometryGenerator geoGen;
	geoGen.CreateGeoSphere(0.5, 3, geoSphere);
	geoGen.CreateSphere(0.5f, 20, 20, sphere);
	geoGen.CreateCylinder(1, 0.3f, 3.0f, 20, 20, cylinder);
	geoGen.CreateFromFile(L"Models/skull.txt", skull);

	mGridObject = new Cube();
	mGridObject->transform->Scale(XMFLOAT3(50, 0.1, 50));

	mCenterObject = new Cube();
	mCenterObject->transform->Translate(XMFLOAT3(0, 1, 0));
	mCenterObject->transform->Scale(XMFLOAT3(0.5, 0.5, 0.5));
	RENDERER(mCenterObject)->LoadGeometry(skull);

	for (int i = 0; i < 5; ++i)
	{
		auto cylinder1 = new Cube();
		cylinder1->transform->Translate(XMFLOAT3(-5.0f, 1.5f, -10.0f + i * 5.0f));
		RENDERER(cylinder1)->LoadGeometry(cylinder);
		auto cylinder2 = new Cube();
		cylinder2->transform->Translate(XMFLOAT3(+5.0f, 1.5f, -10.0f + i * 5.0f));
		RENDERER(cylinder2)->LoadGeometry(cylinder);

		mCylinders.push_back(cylinder1);
		mCylinders.push_back(cylinder2);

		auto sphere1 = new Cube();
		sphere1->transform->Translate(XMFLOAT3(-5.0f, 3.5f, -10.0f + i * 5.0f));
		RENDERER(sphere1)->LoadGeometry(sphere);
		auto sphere2 = new Cube();
		sphere2->transform->Translate(XMFLOAT3(+5.0f, 3.5f, -10.0f + i * 5.0f));
		RENDERER(sphere2)->LoadGeometry(geoSphere);

		mSpheres.push_back(sphere1);
		mSpheres.push_back(sphere2);
	}	
	
	// Build mat
	auto& landMat = RENDERER(mGridObject)->GetMaterial();
	landMat.Ambient = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
	landMat.Diffuse = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
	landMat.Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 16.0f);


	for (auto& cyl : mCylinders)
	{
		auto& mat = RENDERER(cyl)->GetMaterial();
		mat.Ambient = XMFLOAT4(0.7f, 0.85f, 0.7f, 1.0f);
		mat.Diffuse = XMFLOAT4(0.7f, 0.85f, 0.7f, 1.0f);
		mat.Specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 16.0f);
	}

	for (auto& sphere : mSpheres)
	{
		auto& mat = RENDERER(sphere)->GetMaterial();
		mat.Ambient = XMFLOAT4(0.1f, 0.2f, 0.3f, 1.0f);
		mat.Diffuse = XMFLOAT4(0.2f, 0.4f, 0.6f, 1.0f);
		mat.Specular = XMFLOAT4(0.9f, 0.9f, 0.9f, 16.0f);
	}

	auto& skullMat = RENDERER(mCenterObject)->GetMaterial();
	skullMat.Ambient = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	skullMat.Diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	skullMat.Specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 16.0f);
}
