#pragma once

class Timer
{
	friend class System;
	INT64 BaseTime;
	INT64 ticksPerSecond;
	INT64 currentTime;
	INT64 lastTime;
	INT64 lastFPSUpdate;
	INT64 StopTime;
	INT64 PauseTime;

	UINT frameCount;
	float runningTime;
	float framePerSecond;

	float tickCount;
	float timeElapsed;

	bool isStop;
	Timer();
	~Timer();
public:

	void Start();
	void Update();
	void Reset();
	void Stop();
	float GetDelta();
	int GetFrame();
	float RunningTime();
};