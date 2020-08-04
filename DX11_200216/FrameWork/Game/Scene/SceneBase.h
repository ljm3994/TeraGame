#pragma once

class SceneBase
{
public:
	virtual ~SceneBase() = default;
	virtual void SceneInit() = 0;
	virtual void SceneUpdate(float DeltaTime, float TotalTime) = 0;
	virtual void ScenePreRender() = 0;
	virtual void SceneRender() = 0;
	virtual void ScenePostRender() = 0;
	virtual void SceneRelease() = 0;
	virtual void SceneDataLoad() = 0;
	virtual void MapDataLoding() {};
	
};