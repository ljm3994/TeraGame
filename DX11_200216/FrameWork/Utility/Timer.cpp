#include "Framework.h"
#include "Timer.h"

Timer::Timer()
	: currentTime(0), lastFPSUpdate(0), timeElapsed(0),
	frameCount(0), runningTime(0), framePerSecond(0), BaseTime(0),
	PauseTime(0), StopTime(0), isStop(false)
{
	QueryPerformanceFrequency((LARGE_INTEGER*)&ticksPerSecond);
	QueryPerformanceCounter((LARGE_INTEGER*)&lastTime);

	tickCount = 1.0f / ticksPerSecond;
}

Timer::~Timer()
{
}

void Timer::Start()
{
	INT64 StartTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&StartTime);

	if (isStop)
	{
		// 정지되어있던 시간을 구한다
		PauseTime += (StartTime - StopTime);

		lastTime = StartTime;
		StopTime = 0;
		isStop = false;
	}
}

void Timer::Update()
{
	if (isStop)
	{
		timeElapsed = 0.0f;
		return;
	}
	QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);
	timeElapsed = (float)(currentTime - lastTime) * tickCount;
	runningTime += timeElapsed;

	frameCount++;
	if (currentTime - lastFPSUpdate >= ticksPerSecond)
	{
		float tempCurTime = (float)currentTime * tickCount;
		float tempLastTime = (float)lastFPSUpdate * tickCount;
		framePerSecond = (float)frameCount / (tempCurTime - tempLastTime);

		lastFPSUpdate = currentTime;
		frameCount = 0;
	}

	lastTime = currentTime;

	if (timeElapsed < 0.0f)
	{
		timeElapsed = 0.0f;
	}
}

void Timer::Reset()
{
	QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);

	BaseTime = currentTime;
	lastTime = currentTime;
	StopTime = 0;
	PauseTime = 0;
	StopTime = false;
}

void Timer::Stop()
{
	if (!isStop)
	{
		QueryPerformanceCounter((LARGE_INTEGER*)&StopTime);

		isStop = true;
	}
}

float Timer::GetDelta()
{
	return timeElapsed;
}

int Timer::GetFrame()
{
	return (int)framePerSecond;
}

float Timer::RunningTime()
{
	if (isStop)
		return (float)(((StopTime - PauseTime) - BaseTime) * tickCount);

	return (float)(((currentTime - PauseTime) - BaseTime) * tickCount);
}
