#pragma once
#include "Object\TextureRect.h"
#include "FrameWork\States\DepthStencilState.h"
class LoadingScene : public SceneBase
{
private:
	bool firstUpdate;
	promise<void> p;
	future<void> data;
	future_status status;
	string ImageName;
	string NextScene;
	float ImageDelayTime;
	bool loadingstart;
	TextureRect * loadingtexture;
	TextureRect* LoadingBarBack;
	TextureRect* LoadingBarFront;
	TextureRect* LoadingPerImage;
	UINT Percentage;
	UINT TextureIndex;
	UINT TotalTexture;
	DepthStencilState * depthstencile;
	//UINT TotalTexture;
public:
	LoadingScene();
	~LoadingScene();
	virtual void SceneInit() override;
	virtual void SceneUpdate(float DeltaTime, float TotalTime) override;
	virtual void ScenePreRender() override;
	virtual void SceneRender() override;
	virtual void ScenePostRender() override;
	virtual void SceneRelease() override;
	void LoadData();
	future<bool> LoadingModel();
	future<void> ModelLoad(string ModelKey, string ModelPath, MODELTYPE Type, bool isCollision, UINT instaceCount);
	void LodingStart(string NextScene);
	virtual void SceneDataLoad() override;
};