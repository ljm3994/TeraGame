#pragma once
#pragma comment (lib, "Libraries/fmod/fmodex_vc.lib")
#include "Libraries\fmod\fmod.hpp"


//ä�ι��� �� ���� ����
#define EXTRACHANNERBUFFER 5
#define SOUNDBUFFER 100

//�� ���� ���� ���� : ���۰� �۾Ƽ� ������ ������ �ذ�
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