#pragma once
#include "Map\Terrain.h"
#include "Object\Sky.h"
#include "Object\Charter.h"
#include "Object\Monster\DollEnemy.h"
#include <UI\MainUI.h>
#include <UI\Inventory.h>
class TestScene : public SceneBase
{
private:
	Terrain* model;
	Charter* player;
	Sky* sky;
	Enemy * Doll;
	MainUI* ui;
public:
	TestScene();
	~TestScene();
	virtual void SceneInit() override;
	virtual void SceneUpdate(float DeltaTime, float TotalTime) override;
	virtual void ScenePreRender() override;
	virtual void SceneRender() override;
	virtual void ScenePostRender() override;
	virtual void SceneRelease() override;
	virtual void SceneDataLoad() override;
	virtual void MapDataLoding() override;
};