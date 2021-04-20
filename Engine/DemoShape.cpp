#include "stdafx.h"
#include "DemoShape.h"
#include "Renderer.h"
#include "Cube.h"

DemoShape::DemoShape(HINSTANCE hInstance)
: NPGameEngine(hInstance)
, mCenterBox(nullptr)
, mCenterObject(nullptr)
, mGridObject(nullptr)
, mCrateTex(nullptr)
, mFireTexIndex(0)
{
	mCamRadius = 10;
}

DemoShape::~DemoShape()
{

}

bool DemoShape::Init()
{
	if (!NPGameEngine::Init())
		return false;

	mCrateTex = std::make_shared<Texture>(L"Textures/WoodCrate01.dds");
	mDarkBrickTex = std::make_shared<Texture>(L"Textures/darkbrickdxt1.dds");
	XMStoreFloat4x4(&mDarkBrickTex->TexTransform, XMMatrixScaling(10, 5, 0));

	mCylTex = std::make_shared<Texture>(L"Textures/darkbrickdxt1.dds");
	XMStoreFloat4x4(&mCylTex->TexTransform, XMMatrixScaling(2, 2, 0));

	mSphereTex = std::make_shared<Texture>(L"Textures/darkbrickdxt1.dds");
	XMStoreFloat4x4(&mSphereTex->TexTransform, XMMatrixScaling(2, 2, 0));

	for (int i = 1; i <= 120; ++i)
	{
		std::wstring filename(L"Fire.bmp");
		std::wstring count = std::to_wstring(i);
		while (count.length() < 3)
			count.insert(0, 1, L'0');
		filename.insert(filename.begin() + 4, count.begin(), count.end());
		std::wstring filepath(L"Textures/FireAnim/");
		filename.insert(filename.begin(), filepath.begin(), filepath.end());

		mFireTex.push_back(std::make_shared<Texture>(filename));
	}

	BuildGeometryBuffers();

	return true;
}

void DemoShape::UpdateScene()
{
	NPGameEngine::UpdateScene();

	mCenterObject->transform->Rotate(XMFLOAT3(0, 1, 0), GameTimer::DeltaTime());
	mCenterObject->transform->Translate(XMFLOAT3(0, 0.0004 * sin(3 * GameTimer::TotalTime()), 0));

	//for (auto& cyl : mCylinders)
	//{
	//	cyl->transform->Translate(XMFLOAT3(0, 0.02 * sin(50 * GameTimer::TotalTime()), 0));
	//}
	for (auto& sphere : mSpheres)
	{
		sphere->transform->Rotate(XMFLOAT3(0, 1, 0), GameTimer::DeltaTime());
		//sphere->transform->Translate(XMFLOAT3(0, 0.001 * sin(10 * GameTimer::TotalTime()), 0));
	}

	static float animTime = 0;
	animTime += GameTimer::DeltaTime();
	if (animTime > 1.0 / 30)
	{
		RENDERER(mCenterObject)->Texture = mFireTex[mFireTexIndex].get();

		mFireTexIndex++;
		if (mFireTexIndex >= mFireTex.size())
			mFireTexIndex = 0;

		animTime = 0;
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

	mGridObject = std::make_shared<Cube>();
	mGridObject->transform->Scale(XMFLOAT3(15, 0.1, 25));

	RENDERER(mGridObject)->Texture = mDarkBrickTex.get();

	mCenterObject = std::make_shared<Cube>();
	mCenterObject->transform->Translate(XMFLOAT3(0, 2, 0));
	mCenterObject->transform->Scale(XMFLOAT3(3, 3, 3));
	RENDERER(mCenterObject)->Texture = mCrateTex.get();
	//RENDERER(mCenterObject)->LoadGeometry(skull);

	for (int i = 0; i < 5; ++i)
	{
		auto cylinder1 = std::make_shared<Cube>();
		cylinder1->transform->Translate(XMFLOAT3(-5.0f, 1.5f, -10.0f + i * 5.0f));
		RENDERER(cylinder1)->LoadGeometry(cylinder);
		RENDERER(cylinder1)->Texture = mCylTex.get();
		auto cylinder2 = std::make_shared<Cube>();
		cylinder2->transform->Translate(XMFLOAT3(+5.0f, 1.5f, -10.0f + i * 5.0f));
		RENDERER(cylinder2)->LoadGeometry(cylinder);
		RENDERER(cylinder2)->Texture = mCylTex.get();

		mCylinders.push_back(cylinder1);
		mCylinders.push_back(cylinder2);

		auto sphere1 = std::make_shared<Cube>();
		sphere1->transform->Translate(XMFLOAT3(-5.0f, 3.5f, -10.0f + i * 5.0f));
		RENDERER(sphere1)->LoadGeometry(sphere);
		RENDERER(sphere1)->Texture = mSphereTex.get();
		auto sphere2 = std::make_shared<Cube>();
		sphere2->transform->Translate(XMFLOAT3(+5.0f, 3.5f, -10.0f + i * 5.0f));
		RENDERER(sphere2)->LoadGeometry(geoSphere);
		RENDERER(sphere2)->Texture = mSphereTex.get();

		mSpheres.push_back(sphere1);
		mSpheres.push_back(sphere2);
	}	
	
	// Build mat
	auto& landMat = RENDERER(mGridObject)->GetMaterial();
	landMat.Ambient = XMFLOAT4(0.48f, 0.48f, 0.48f, 1.0f);
	landMat.Diffuse = XMFLOAT4(0.48f, 0.48f, 0.48f, 1.0f);
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
