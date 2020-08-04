#pragma once
#include "FrameWork/Game/Scene/SceneBase.h"

class TerrainEditorScene : public SceneBase
{
public:
	TerrainEditorScene();
	~TerrainEditorScene();

	virtual void SceneInit() override;
	virtual void SceneUpdate(float DeltaTime, float TotalTime) override;
	virtual void ScenePreRender() override;
	virtual void SceneRender() override;
	virtual void ScenePostRender() override;
	virtual void SceneRelease() override;

	// SceneBase��(��) ���� ��ӵ�
	virtual void SceneDataLoad() override;
	virtual void MapDataLoding() override;
};