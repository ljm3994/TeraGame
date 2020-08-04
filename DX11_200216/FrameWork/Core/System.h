#pragma once
#include "FrameWork/Utility/Timer.h"
#include "FrameWork/Utility/Cpu.h"

class System
{
protected:
	HWND hWnd;
	HINSTANCE instance;
	bool isAppPaused;           //애플리케이션 정지
	bool isMinmized;            //창 최소화
	bool isMaxMized;            //창 최대화
	bool isResizing;            //창 크기 변경
	bool isEnable4xMsaa;        //안티에일리어싱 가능 여부
	UINT MsaaQulity;
	wstring AppName;
	UINT ClientWidth;
	UINT ClientHeight;

	Timer Time;
	CPU m_Cpu;
	virtual bool AppInit();
	virtual void SizeChanged(WPARAM wparam) = 0;
	virtual void OnResize() = 0;
	virtual void Update() = 0;
	virtual void Render() = 0;
public:
	System(HINSTANCE hinstance, wstring AppName = L"DX11", bool Enable4xMsaa = true);
	virtual ~System();

	virtual LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
	virtual int Run() = 0;
private:
	ATOM MyRegisterClass();
	BOOL InitInstance();
};