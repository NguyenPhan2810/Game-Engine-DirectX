#include "stdafx.h"
#include "DemoShape.h"
#include "Renderer.h"
#include "Cube.h"

DemoShape::DemoShape(HINSTANCE hInstance)
: NPGameEngine(hInstance)
, mCenterBox(nullptr)
, mCenterObject(nullptr)
, mGridObject(nullptr)
, mCrateTex()
{
	mCamRadius = 10;
}

DemoShape::~DemoShape()
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

bool DemoShape::Init()
{
	if (!NPGameEngine::Init())
		return false;

	BuildGeometryBuffers();

	return true;
}

void DemoShape::UpdateScene()
{
	NPGameEngine::UpdateScene();

	mCenterObject->transform->Rorate(XMFLOAT3(0, 1, 0), GameTimer::DeltaTime());
	mCenterObject->transform->Translate(XMFLOAT3(0, 0.0004 * sin(3 * GameTimer::TotalTime()), 0));

	for (auto& sphere : mSpheres)
	{
		sphere->transform->Translate(XMFLOAT3(0, 0.01 * sin(100 * GameTimer::TotalTime()), 0));
	}
}

void DemoShape::BuildGeometryBuffers()
{
	GeometryGenerator::MeshData sphere;
	GeometryGenerator::MeshData geoSphere;
	GeometryGenerator::MeshData cylinder;
	GeometryGenerator::MeshData skull;

	GeometryGenerator geoGen;
	geoGen.CreateGeoSphere(0.8, 3, geoSphere);
	geoGen.CreateSphere(0.8f, 20, 20, sphere);
	geoGen.CreateCylinder(0.7, 0.3f, 3.0f, 20, 20, cylinder);
	geoGen.CreateFromFile(L"Models/skull.txt", skull);

	mGridObject = new Cube();
	mGridObject->transform->Scale(XMFLOAT3(50, 0.1, 50));

	mCenterObject = new Cube();
	mCenterObject->transform->Translate(XMFLOAT3(0, 2, 0));
	mCenterObject->transform->Scale(XMFLOAT3(3, 3, 3));
	mCrateTex = std::make_shared<Texture>(Texture(L"Textures/WoodCrate01.dds"));
	RENDERER(mCenterObject)->Texture = mCrateTex;
	//RENDERER(mCenterObject)->LoadGeometry(skull);

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
}
