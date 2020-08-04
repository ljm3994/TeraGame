#include "Framework.h"
#include "LoadingScene.h"
#include <Object\Charter.h>
#include "Map\Terrain.h"
#include <Object\Sky.h>
#include <FrameWork\Assimp\AssimpFactory.h>
#include "Object\Monster/DollEnemy.h"

LoadingScene::LoadingScene()
{
	TotalTexture = 0;
}

LoadingScene::~LoadingScene()
{
}

void LoadingScene::SceneInit()
{
	//AssimpFactory::MapDataExporter("ModelData/FBXFile/clouds.obj", "Sky/Cloude", true);
	//AssimpFactory::MapDataExporter("ModelData/FBXFile/SkyCube.obj", "Sky/Sky", true);
	//auto model1 = AssimpFactory::ExtractSkinnedMesh("ModelData/FBXFile/Elin25.fbx", "Elin25/Event25Body");
	////auto model2 = AssimpFactory::ExtractSkinnedMesh("ModelData/FBXFile/Event11Lod.fbx", "Elin11/Event11BodyLod");
	////auto model3 = AssimpFactory::ExtractSkinnedMesh("ModelData/FBXFile/ElinHair.fbx", "Elin80/ElinHair80", true);
	////auto model4 = AssimpFactory::ExtractSkinnedMesh("ModelData/FBXFile/ElinTail.fbx", "Elin80/ElinTail80", true);
	////////////////////////
	//AssimpFactory::AddAnimation(model1, "ModelData/FBXFile/BodyDualWeaponWait.fbx", "Elin25/ElinDualWeaponAnim/ElinBodyWeaponWait", true, true);
	//////////AssimpFactory::AddAnimation(model1, "ModelData/BodyOutWeapon.fbx", "ElinWeaponAnim/ElinBodyOutWeapon", true);
	//AssimpFactory::AddAnimation(model2, "ModelData/FBXFile/FaceDualWeaponCombo4.fbx", "ElinDualWeaponAnim/ElinFaceWeaponCombo4", true, true);
	//////////AssimpFactory::AddAnimation(model2, "ModelData/FaceOutWeapon.fbx", "ElinWeaponAnim/ElinFaceOutWeapon", true);
	//AssimpFactory::AddAnimation(model3, "ModelData/FBXFile/HairDualWeaponCombo4.fbx", "ElinDualWeaponAnim/ElinHairWeaponCombo4", true, true);
	//////////AssimpFactory::AddAnimation(model3, "ModelData/HairOutWeapon.fbx", "ElinWeaponAnim/ElinHairOutWeapon", true);
	//AssimpFactory::AddAnimation(model4, "ModelData/FBXFile/TailDualWeaponCombo4.fbx", "ElinDualWeaponAnim/ElinTailWeaponCombo4", true, true);
	//AssimpFactory::AddAnimation(model4, "ModelData/TailOutWeapon.fbx", "ElinWeaponAnim/ElinTailOutWeapon", true);
	//
	//delete model1;
	//delete model2;
	//delete model3;
	//delete model4;
	Engine->SetLoading(true);
	status = future_status::timeout;
	ImageDelayTime = 0.0f;
	TextureIndex = 0;
	Percentage = 0;
	firstUpdate = false;
	loadingstart = false;
	LoadingBarBack = new TextureRect();
	LoadingBarFront = new TextureRect();
	LoadingPerImage = new TextureRect();
	Engine->AddTexture("LoadingBack", L"Texture/LoadingImage/LodingBarBack.png");
	Engine->AddTexture("LoadingFront", L"Texture/LoadingImage/LoadingBarFront.png");
	Engine->AddTexture("LoadingPer", L"Texture/LoadingImage/LoadingEffect.png");
	Engine->GetSoundSystem()->AddSound("LodingSound", "Sound/LobbyThema.ogg", true);
	Engine->GetSoundSystem()->Play("LodingSound");
	//LoadingBarBack->ChangeTexture("LoadingBack");
	//LoadingBarFront->ChangeTexture("LoadingFront");
	//LoadingPerImage->ChangeTexture("LoadingPer");

	loadingtexture = new TextureRect();
	Engine->AddCamera("Default", CarmeraType::DEFAULTCARMERA)->SetEye(Engine->GetClientWidth() * 0.5f, Engine->GetClientHeight() * 0.5f, -800.0f);
	Engine->ChangeCamera("Default");
	depthstencile = new DepthStencilState();
	depthstencile->DepthEnable(false);
	//LoadingBarBack->SetScale(700, 70.0f, 0.0f);
	//LoadingBarFront->SetScale(100.0f, 30.0f, 0.0f);
	//LoadingBarFront->SetPosition(Engine->GetClientWidth() * 0.5f, 70, 1.0f);
	//LoadingBarBack->SetPosition(Engine->GetClientWidth() * 0.5f, 60, 1.0f);
	loadingtexture->SetScale(Engine->GetClientWidth(), Engine->GetClientHeight(), 0.0f);
	loadingtexture->SetPosition(Engine->GetClientWidth() * 0.5f, Engine->GetClientHeight() * 0.5f, 0.0f);
}

void LoadingScene::SceneUpdate(float DeltaTime, float TotalTime)
{
	if (loadingstart)
	{
		if (!firstUpdate)
		{
			data = async([&]() {LoadData(); });
			firstUpdate = true;
		}

		status = data.wait_for(chrono::milliseconds(10));
		if (status == future_status::ready)
		{
			depthstencile->DepthEnable(true);
			depthstencile->Set();
			Engine->GetSoundSystem()->Stop("LodingSound");
			Engine->GetSoundSystem()->Play("ScenSound1");
			Engine->SetLoading(false);
			Engine->ChangeScene(NextScene);
			
		}
		else if (status == future_status::timeout)
		{
			ImageDelayTime += DeltaTime;
			loadingtexture->Update();
			//LoadingBarBack->Update();
			//LoadingBarFront->Update();
			//LoadingPerImage->Update();
			if (ImageDelayTime > 2.0f)
			{
				ImageDelayTime = 0.0f;
				TextureIndex++;
				if (TextureIndex >= TotalTexture)
				{
					TextureIndex = 0;
				}
				string index = std::to_string(TextureIndex);
				string tag = "Texture" + index;
				loadingtexture->ChangeTexture(tag);
			}
		}
	}
}

void LoadingScene::ScenePreRender()
{
}

void LoadingScene::SceneRender()
{
	if (status == future_status::timeout)
	{
		depthstencile->Set();
		loadingtexture->Render();
		//LoadingBarBack->Render();
		//LoadingBarFront->Render();
		//LoadingPerImage->Render();
	}
}

void LoadingScene::ScenePostRender()
{
}

void LoadingScene::SceneRelease()
{
	delete loadingtexture;
	delete depthstencile;
	delete LoadingBarBack;
	delete LoadingBarFront;
	delete LoadingPerImage;
}

void LoadingScene::LoadData()
{
	Engine->AddTexture("Texture0", L"Texture/LoadingImage/LoadingImage4_Tex.png");
	TotalTexture++;
	Engine->AddTexture("Texture1", L"Texture/LoadingImage/LoadingImage6_Tex.png");
	TotalTexture++;
	Engine->AddTexture("Texture2", L"Texture/LoadingImage/LoadingImage7_Tex.png");
	TotalTexture++;
	Engine->AddTexture("Texture3", L"Texture/LoadingImage/LoadingImage8_Tex.png");
	TotalTexture++;
	Engine->AddTexture("Texture4", L"Texture/LoadingImage/LoadingImage9_Tex.png");
	TotalTexture++;
	Engine->AddTexture("Texture5", L"Texture/LoadingImage/LoadingImage10_Tex.png");
	TotalTexture++;
	Engine->AddTexture("Texture6", L"Texture/LoadingImage/LoadingImage11_Tex.png");
	TotalTexture++;
	Engine->AddTexture("Texture7", L"Texture/LoadingImage/LoadingImage12_Tex.png");
	TotalTexture++;
	Engine->AddTexture("Texture8", L"Texture/LoadingImage/LoadingImage13_Tex.png");
	Percentage++;
	Engine->AddTexture("Event75", L"Texture/Icon/PC_Event_75_F_B_Tex.png");
	Percentage++;
	Engine->AddTexture("Event80", L"Texture/Icon/PC_Event_80_F_B_Tex.png");
	Percentage++;
	Engine->AddTexture("Event05", L"Texture/Icon/PC_Event05_Popori_F_Tex.png");
	Percentage++;
	Engine->AddTexture("Event11", L"Texture/Icon/PC_Event_11_Popori_F_03_Black_Tex.png");
	Percentage++;
	Engine->AddTexture("Event17", L"Texture/Icon/PC_Event_17_Popori_F_Tex.png");
	Percentage++;
	Engine->AddTexture("Event21", L"Texture/Icon/PC_Event_21A_Popori_F_A_Tex.png");
	Percentage++;
	Engine->AddTexture("Event25", L"Texture/Icon/PC_Event_25_Popori_F_N_B_Tex.png");
	Percentage++;
	Engine->AddTexture("Rapid1", L"Texture/Icon/RapidAttack_Tex.png");
	Percentage++;
	Engine->AddTexture("Rapid2", L"Texture/Icon/dualrapidpiercing_Tex.png");
	Percentage++;
	Engine->AddTexture("Rapid3", L"Texture/Icon/C12_RapidSlash.png");
	Percentage++;

	auto result = LoadingModel();
	result.wait();

	Engine->GetSoundSystem()->AddSound("AttkSound", "Sound/Popori_F_Attack.ogg", false);
	if (!Engine->GetComponentObject<Charter>("Player"))
	{
		Charter* charter = new Charter();
		charter->CharterInit("Body", "Face", "Hair", "Tail", "Rweapon", "Lweapon");
		Engine->AddObject("Player", charter);
	}
	if (!Engine->GetComponentObject<Enemy>("DollEnemy"))
	{
		DollEnemy* Doll = new DollEnemy();
		Engine->AddObject("DollEnemy", Doll);
	}
	Engine->FindScene(NextScene)->SceneInit();
	Percentage++;
	Engine->FindScene(NextScene)->MapDataLoding();
	Percentage++;

	return;
}

future<bool> LoadingScene::LoadingModel()
{
	//"Elin80/ElinFace80", "Elin80/ElinHair80", "Elin80/ElinTail80", ""
	co_await ModelLoad("Body", "Elin80/ElinBody80", MODELTYPE::SKINNING_MODELTYPE, true, 1);
	Percentage++;
	co_await ModelLoad("Body11", "Elin11/Event11Body", MODELTYPE::SKINNING_MODELTYPE, true, 1);
	Percentage++;
	co_await ModelLoad("Body17", "Elin17/Event17Body", MODELTYPE::SKINNING_MODELTYPE, true, 1);
	Percentage++;
	co_await ModelLoad("Body21", "Elin21/Event21Body", MODELTYPE::SKINNING_MODELTYPE, true, 1);
	Percentage++;
	co_await ModelLoad("Body25", "Elin25/Event25Body", MODELTYPE::SKINNING_MODELTYPE, true, 1);
	Percentage++;
	co_await ModelLoad("Face", "Elin80/ElinFace80", MODELTYPE::SKINNING_MODELTYPE, false, 1);
	Percentage++;
	co_await ModelLoad("Hair", "Elin80/ElinHair80", MODELTYPE::SKINNING_MODELTYPE, false, 1);
	Percentage++;
	co_await ModelLoad("Tail", "Elin80/ElinTail80", MODELTYPE::SKINNING_MODELTYPE, false, 1);
	Percentage++;
	co_await ModelLoad("Doll", "Monster/Doll", MODELTYPE::SKINNING_MODELTYPE, true, 10);
	Percentage++;
	co_await ModelLoad("Lweapon", "WeaponDual39/DualWeaponL39", MODELTYPE::STATIC_MODELTYPE, true, 1);
	Percentage++;
	co_await ModelLoad("Rweapon", "WeaponDual39/DualWeaponR39", MODELTYPE::STATIC_MODELTYPE, true, 1);
	Percentage++;
	co_await ModelLoad("Terrain", "Map/Map", MODELTYPE::STATIC_MODELTYPE, false, 1);
	Percentage++;
	co_await ModelLoad("Sky", "Sky/Sky", MODELTYPE::STATIC_MODELTYPE, false, 1);
	co_return true;
}

future<void> LoadingScene::ModelLoad(string ModelKey, string ModelPath, MODELTYPE Type, bool isCollision, UINT instaceCount)
{
	Engine->AddModel(ModelKey, ModelPath, Type, isCollision, instaceCount);
	
	co_return;
}

void LoadingScene::LodingStart(string NextScene)
{
	loadingstart = true;
	this->NextScene = NextScene;
}

void LoadingScene::SceneDataLoad()
{
}
