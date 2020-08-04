#pragma once
#pragma comment (lib, "Libraries/fmod/fmodex_vc.lib")
#include "Libraries\fmod\fmod.hpp"


//채널버퍼 및 사운드 버퍼
#define EXTRACHANNERBUFFER 5
#define SOUNDBUFFER 100

//총 사운드 버퍼 설정 : 버퍼가 작아서 씹히는 문제를 해결
#define TOTALSOUNDBUFFER SOUNDBUFFER * EXTRACHANNERBUFFER

class GameSound
{
	FMOD::System* system;		
	FMOD::Sound** sound;
	FMOD::Channel** channel;
	unordered_map<string, FMOD::Sound**> totalSound;
	unordered_map<string, FMOD::Channel*> totalChannel;
	float m_Pitch;
public:
	GameSound();
	~GameSound();
	void AddSound(string Name, string FilePath, bool isLoop);
	void Play(string Name, float volum = 1.0f);

	void Stop(string Name);

	void Pause(string Name);
	void Resum(string Name);
	bool isPauseSound(string Name);

	void Update();
};