#include "Framework.h"
#include "Sound.h"

GameSound::GameSound()
{
	System_Create(&system);

	system->init(TOTALSOUNDBUFFER, FMOD_INIT_NORMAL, 0);

	sound = new FMOD::Sound * [TOTALSOUNDBUFFER];
	channel = new FMOD::Channel * [TOTALSOUNDBUFFER];

	m_Pitch = 1.0f;
	memset(sound, 0, sizeof(FMOD::Sound*) * (TOTALSOUNDBUFFER));
	memset(channel, 0, sizeof(FMOD::Channel*) * (TOTALSOUNDBUFFER));
}

GameSound::~GameSound()
{
	if (channel != nullptr || sound != nullptr)
	{
		for (int i = 0; i < TOTALSOUNDBUFFER; i++)
		{
			if (channel != nullptr)
			{
				if (channel[i]) channel[i]->stop();
			}

			if (sound != nullptr)
			{
				if (sound[i]) sound[i]->release();
			}
		}
	}

	delete channel;
	delete sound;

	if (system != nullptr)
	{
		system->release();
		system->close();
	}
}

void GameSound::Play(string Name, float volum)
{
	bool isPlay;
	int count = 0;

	if (totalChannel.count(Name) == 0)
		return;

	//기존 사운드 재생중인지 확인
	totalChannel[Name]->isPlaying(&isPlay);
	if (isPlay)return;
	//사운드 플레이
	system->playSound(FMOD_CHANNEL_FREE, *totalSound[Name], false, &totalChannel[Name]);
	//볼륨셋팅
	totalChannel[Name]->setVolume(volum);
}

void GameSound::Stop(string Name)
{
	bool isPlay;
	int count = 0;
	if (totalChannel.count(Name) == 0)
		return;
	//기존 사운드 재생중인지 확인
	totalChannel[Name]->isPlaying(&isPlay);
	if (!isPlay)return;

	totalChannel[Name]->stop();
}

void GameSound::Pause(string Name)
{
	bool isPause;
	int count = 0;
	if (totalChannel.count(Name) == 0)
		return;
	totalChannel[Name]->getPaused(&isPause);
	if (isPause)return;
	totalChannel[Name]->setPaused(true);
}

void GameSound::Resum(string Name)
{
	bool isPause;
	int count = 0;
	if (totalChannel.count(Name) == 0)
		return;

	totalChannel[Name]->getPaused(&isPause);
	if (!isPause)return;
	totalChannel[Name]->setPaused(false);
}

bool GameSound::isPauseSound(string Name)
{
	return false;
}

void GameSound::Update()
{
	system->update();
}

void GameSound::AddSound(string Name, string FilePath, bool isLoop)
{
	if (isLoop)
	{
		system->createSound(FilePath.c_str(), FMOD_LOOP_NORMAL, nullptr, &sound[totalSound.size()]);
	}
	else
	{
		system->createSound(FilePath.c_str(), FMOD_DEFAULT, nullptr, &sound[totalSound.size()]);
	}

	totalSound.insert(make_pair(Name, &sound[totalSound.size()]));
	totalChannel.insert(make_pair(Name, channel[totalSound.size()]));
}
