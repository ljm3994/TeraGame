#include "Framework.h"
#include "IntroScene.h"
#include <Scene\LoadingScene.h>

IntroScene::IntroScene()
{
	isFirst = false;

	Engine->GetSoundSystem()->AddSound("IntroSound", "Video/S_Soulless_BGStory.ogg", false);
}

IntroScene::~IntroScene()
{
}

void IntroScene::SceneInit()
{

}

void IntroScene::SceneUpdate(float DeltaTime, float TotalTime)
{
	if (!isFirst)
	{
		isFirst = true;
		_TCHAR str[256] = _T("Video/S_Soulless_BGStory.wmv");
		VIDEOMANAGER->AddMovie("Intro", str, 0, 0, 0, 0);
		VIDEOMANAGER->Play("Intro");
		Engine->GetSoundSystem()->Play("IntroSound");
		return;
	}

	if (VIDEOMANAGER->MovieCanPlay())
	{
		if (VIDEOMANAGER->isEnd())
		{
			VIDEOMANAGER->Stop();
			Engine->GetSoundSystem()->Stop("IntroSound");
		}
		if (InputKey->GetState().Enter)
		{
			VIDEOMANAGER->Stop();
			Engine->GetSoundSystem()->Stop("IntroSound");
		}
	}
	else
	{
		dynamic_cast<LoadingScene*>(Engine->FindScene("Loading"))->SceneInit();
		dynamic_cast<LoadingScene*>(Engine->FindScene("Loading"))->LodingStart("Test");
		Engine->ChangeScene("Loading");
	}
}

void IntroScene::ScenePreRender()
{
}

void IntroScene::SceneRender()
{
}

void IntroScene::ScenePostRender()
{
}

void IntroScene::SceneRelease()
{
}

void IntroScene::SceneDataLoad()
{
}
