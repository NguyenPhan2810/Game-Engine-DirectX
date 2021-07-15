

#include "DemoShape.h"
#include "Renderer.h"
#include "Cube.h"

DemoShape::DemoShape(HINSTANCE hInstance)
: NPGameEngine(hInstance)
, mCenterBox(nullptr)
, mCrateObject(nullptr)
, mFloorObject(nullptr)
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

	CreateObjects();
	CreateTextures();
	CreateMaterials();

	
	return true;
}

void DemoShape::UpdateScene()
{
	NPGameEngine::UpdateScene();

	mCrateObject->transform->Rotate(XMFLOAT3(0, 1, 0), GameTimer::DeltaTime());
	mCrateObject->transform->Translate(XMFLOAT3(0, 0.00005 * sin(2 * GameTimer::TotalTime()), 0));


	static float animTime = 0;
	animTime += GameTimer::DeltaTime();
	if (animTime > 1.0 / 30)
	{
		RENDERER(mCrateObject)->Texture = mFireTex[mFireTexIndex].get();

		mFireTexIndex++;
		if (mFireTexIndex >= mFireTex.size())
			mFireTexIndex = 0;

		animTime = 0;
	}
}

void DemoShape::CreateObjects()
{
	GeometryGenerator::MeshData sphereMesh;
	GeometryGenerator::MeshData geoSphereMesh;
	GeometryGenerator::MeshData cylinderMesh;
	GeometryGenerator::MeshData skullMesh;

	GeometryGenerator geoGen;
	geoGen.CreateGeoSphere(0.8, 3, geoSphereMesh);
	geoGen.CreateSphere(0.8f, 20, 20, sphereMesh);
	geoGen.CreateCylinder(0.7, 0.3f, 3.0f, 20, 20, cylinderMesh);
	geoGen.CreateFromFile(L"Models/skull.txt", skullMesh);

	//
	// Center object
	//

	mCrateObject = std::make_shared<Cube>();
	mCrateObject->transform->Translate(XMFLOAT3(0, 2, 0));
	mCrateObject->transform->Scale(XMFLOAT3(2, 2, 2));
	//RENDERER(mCrateObject)->LoadGeometry(skull);
	
	//
	// Floor
	//

	mFloorObject = std::make_shared<Cube>();
	mFloorObject->transform->Scale(XMFLOAT3(15, 0.1, 25));
	XMStoreFloat4x4(&RENDERER(mFloorObject)->TexTransform, XMMatrixScaling(6, 8, 0));

	//
	// Build cylinder and sphere
	//
	
	for (float x = -5; x <= 5; x += 10)
		for (int i = 0; i < 5; ++i)
		{
			auto cylinder = std::make_shared<Cube>();
			cylinder->transform->Translate(XMFLOAT3(x, 1.5f, -10.0f + i * 5.0f));
			RENDERER(cylinder)->LoadGeometry(cylinderMesh);
			RENDERER(cylinder)->Texture = mCylTex.get();
			XMStoreFloat4x4(&RENDERER(cylinder)->TexTransform, XMMatrixScaling(2, 2, 0));

			mCylinders.push_back(cylinder);

			auto sphere = std::make_shared<Cube>();
			sphere->transform->Translate(XMFLOAT3(x, 3.5f, -10.0f + i * 5.0f));
			RENDERER(sphere)->LoadGeometry(sphereMesh);
			RENDERER(sphere)->Texture = mSphereTex.get();
			XMStoreFloat4x4(&RENDERER(sphere)->TexTransform, XMMatrixScaling(2, 2, 0));

			mSpheres.push_back(sphere);
		}
}

void DemoShape::CreateTextures()
{
	mCrateTex = std::make_shared<Texture>(L"Textures/WoodCrate01.dds");
	mDarkBrickTex = std::make_shared<Texture>(L"Textures/darkbrickdxt1.dds");
	mCylTex = mDarkBrickTex;
	mSphereTex = mDarkBrickTex;

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


	RENDERER(mFloorObject)->Texture = mDarkBrickTex.get();
	RENDERER(mCrateObject)->Texture = mCrateTex.get();	
	for (auto& cyl : mCylinders)
		RENDERER(cyl)->Texture = mCylTex.get();
	for (auto& sphere : mSpheres)
		RENDERER(sphere)->Texture = mSphereTex.get();
}

void DemoShape::CreateMaterials()
{
	// Build mat
	auto& landMat = RENDERER(mFloorObject)->GetMaterial();
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