#include "Framework.h"
#include "TestScene.h"
#include "FrameWork\Assimp\AssimpFactory.h"
#include <FrameWork\Environment\Camera\FollowCarmera.h>
TestScene::TestScene()
{

}

TestScene::~TestScene()
{
	delete model;
	delete ui;
}

void TestScene::SceneInit()
{
	//AssimpFactory::MapDataExporter("ModelData/WeaponAxe38.fbx", "WeaponAxe38/WeaponAxe38", true);
	//auto model1 = AssimpFactory::ExtractSkinnedMesh("ModelData/FBXFile/DollMonster.fbx", "Monster/Doll");
	//auto model2 = AssimpFactory::ExtractSkinnedMesh("ModelData/ElinFace.fbx", "Elin80/ElinFace80", true);
	//auto model3 = AssimpFactory::ExtractSkinnedMesh("ModelData/ElinHair.fbx", "Elin80/ElinHair80", true);
	//auto model4 = AssimpFactory::ExtractSkinnedMesh("ModelData/ElinTail.fbx", "Elin80/ElinTail80", true);
	////////
	//AssimpFactory::AddAnimation(model1, "ModelData/FBXFile/DollMonsterDeathWait.fbx", "Monster/DollDeathWait", true);
	//AssimpFactory::AddAnimation(model2, "ModelData/ElinFaceWalkWalk.fbx", "Elin80/ElinFaceWark", true);
	//AssimpFactory::AddAnimation(model3, "ModelData/ElinHairWalkWalk.fbx", "Elin80/ElinHairWark", true);
	//AssimpFactory::AddAnimation(model4, "ModelData/ElinTailWalkWalk.fbx", "Elin80/ElinTailWark", true);
	//
	//delete model1;
	//delete model2;
	//delete model3;
	//delete model4;
	Engine->GetSoundSystem()->AddSound("ScenSound1", "Sound/Refuge_Village.ogg", true);
	SceneDataLoad();
	Engine->AddCamera("Follow", CarmeraType::FOLLOWCAMERA);
	Engine->ChangeCamera("Follow");
}

void TestScene::SceneUpdate(float DeltaTime, float TotalTime)
{
	sky->Update(DeltaTime);
	model->Update();
	player->Update(DeltaTime);
	Doll->Update(DeltaTime);

	D3DXVECTOR3 playerpos = player->GetTransform()->GetPosition();
	playerpos.y += 1.0f;
	D3DXVECTOR3 OutPos;
	if (model->GetHeight(playerpos, OutPos))  // 플레이어가 지형과 충돌 했다면
	{
		player->GetTransform()->SetPosition(playerpos.x, OutPos.y, playerpos.z);
	}

	ui->SetEnemyHpShow(player->GetIsEnemyColl(), DeltaTime);
	ui->SetPlayerInfo(player->GetPlayerHp(), player->GetPlayMaxHp(), player->GetPlayerMp(), player->GetPlayMaxMp());
	ui->SetEnemyInfo(Doll->GetMonsterCurrentHP(), Doll->GetMonsterMaxHP());
}

void TestScene::ScenePreRender()
{

}

void TestScene::SceneRender()
{
	model->Render();
	player->Render();
	Doll->Render();
}

void TestScene::ScenePostRender()
{
	player->BaseObjectPostRender();
	ui->Render();
	sky->Render();
}

void TestScene::SceneRelease()
{
}

void TestScene::SceneDataLoad()
{
}

void TestScene::MapDataLoding()
{
	model = new Terrain("Terrain");
	model->LoadTerrainCells();
	player = Engine->GetComponentObject<Charter>("Player");
	player->GetTransform()->SetPosition(70.0f, 20.0f, 530.0f);
	player->GetTransform()->SetScale(0.1f, 0.1f, 0.1f);
	Engine->FindCamera<FollowCamera>("Follow")->SetTarget(player->GetTransform(), 0.0f);
	player->SetActive(true);
	Doll = Engine->GetComponentObject<Enemy>("DollEnemy");
	Doll->SetPlayerTransform(player->GetTransform());
	Doll->SetTerrain(model);
	Doll->SetPlayerColl(player);
	player->SetEnemy(Doll);
	for (UINT i = 0; i < 10; i++)
	{
		Doll->GetMonsterPosition(i)->SetPosition(100.0f, 20.0f, 530.0f + (i * 1.5f));

		Doll->GetMonsterPosition(i)->SetScale(0.1f, 0.1f, 0.1f);
	}
	sky = new Sky("Sky");
	
	Doll->SetActive(true);

	ui = new MainUI();
}
