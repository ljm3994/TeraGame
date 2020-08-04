#pragma once
#include "FrameWork/Utility/Timer.h"
#include "FrameWork/Utility/Cpu.h"

class System
{
protected:
	HWND hWnd;
	HINSTANCE instance;
	bool isAppPaused;           //���ø����̼� ����
	bool isMinmized;            //â �ּ�ȭ
	bool isMaxMized;            //â �ִ�ȭ
	bool isResizing;            //â ũ�� ����
	bool isEnable4xMsaa;        //��Ƽ���ϸ���� ���� ����
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