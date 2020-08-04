#pragma once

class IntroScene : public SceneBase
{
private:
	bool isFirst;
public:
	IntroScene();
	~IntroScene();

	virtual void SceneInit() override;
	virtual void SceneUpdate(float DeltaTime, float TotalTime) override;
	virtual void ScenePreRender() override;
	virtual void SceneRender() override;
	virtual void ScenePostRender() override;
	virtual void SceneRelease() override;
	virtual void SceneDataLoad() override;
};