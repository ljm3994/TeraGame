#pragma once

#include "Framework/Core/D3DDevice.h"
#include "MainProgram/MainProgram.h"

class Application : public D3DDevice
{
private:
	unique_ptr<MainProgram> program;

	virtual void Update() override;
	virtual void Render() override;
public:
	static bool isSystemInfoOpen;

	Application(HINSTANCE hinstance, wstring AppName = L"DX11", bool Enable4xMsaa = true, bool isVsync = false);
	~Application();
	bool Init();
	void ImGuiInit();
	void ImguiSystemInfo(bool* Open);
};