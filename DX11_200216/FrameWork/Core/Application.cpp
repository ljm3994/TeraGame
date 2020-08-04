#include "Framework.h"
#include "Application.h"

bool Application::isSystemInfoOpen = false;

void Application::Update()
{
	//기본함수 업데이트
	//////////////////////////////////////
	program->Update(RenderTargetView.Get(), ClientWidth, ClientHeight, Time.GetDelta(), Time.RunningTime());

	if (!VIDEOMANAGER->GetisPlay())
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}
}

void Application::Render()
{
	if (!VIDEOMANAGER->GetisPlay())
	{
		BeginRender(D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f));
		program->PreRender();
		program->Render();
		program->PostRender();
		//ImguiSystemInfo(&isSystemInfoOpen);
		//postprocess->Render(Time.RunningTime(), Time.GetDelta(), RenderTargetView.Get());
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
		ImGuiIO& io = ImGui::GetIO();

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}

		EndRender();
	}
}

Application::Application(HINSTANCE hinstance, wstring AppName, bool Enable4xMsaa, bool isVsync)
	: D3DDevice(hinstance, AppName, Enable4xMsaa, isVsync)
{
}

Application::~Application()
{
	ImGui::DestroyContext();
	program.reset();
}

bool Application::Init()
{
	if (!InitDevice())
		return false;

	VIDEOMANAGER->SetWindowHwnd(ClientWidth, ClientHeight, hWnd, instance);
	ImGuiInit();
	program = make_unique<MainProgram>(MainProgram());
	program->Init();

	return true;
}

void Application::ImGuiInit()
{
	ImGui_ImplWin32_EnableDpiAwareness();

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; 
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	//io.ConfigViewportsNoAutoMerge = true;
	//io.ConfigViewportsNoTaskBarIcon = true;
	//io.ConfigViewportsNoDefaultParent = true;
	//io.ConfigDockingAlwaysTabBar = true;
	//io.ConfigDockingTransparentPayload = true;
	io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleFonts;
	io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleViewports;

	ImGui::CreateContext();

	ImGui::StyleColorsDark();

	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 0.0f;
	}

	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX11_Init(DXDevice.Get(), Context.Get());
}

void Application::ImguiSystemInfo(bool* Open)
{
	const float DISTANCE = 10.0f;
	static int corner = 0;

	ImGuiIO& io = ImGui::GetIO();
	if (corner != -1)
	{
		ImVec2 window_pos = ImVec2((corner & 1) ? io.DisplaySize.x - DISTANCE : DISTANCE, (corner & 2) ? io.DisplaySize.y - DISTANCE : DISTANCE);
		ImVec2 window_pos_pivot = ImVec2((corner & 1) ? 1.0f : 0.0f, (corner & 2) ? 1.0f : 0.0f);
		ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
	}

	ImGui::SetNextWindowBgAlpha(0.35f);

	if (ImGui::Begin(u8"시스템 정보", Open, (corner != -1 ? ImGuiWindowFlags_NoMove : 0) | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoTitleBar))
	{
		ImGui::Text(u8"Cpu 사용률 : %d %", m_Cpu.GetPercentage());
		ImGui::Text(u8"FPS : %d", Time.GetFrame());
		ImGui::Text(u8"실행시간 : %f", Time.RunningTime());
		ImGui::Text(u8"마우스 좌표 : (%d, %d)", InputMouse->GetState().x, InputMouse->GetState().y);

		if (ImGui::BeginPopupContextWindow())
		{
			if (ImGui::MenuItem(u8"자유이동", NULL, corner == -1)) corner = -1;
			if (ImGui::MenuItem(u8"좌상단", NULL, corner == 0)) corner = 0;
			if (ImGui::MenuItem(u8"우상단", NULL, corner == 1)) corner = 1;
			if (ImGui::MenuItem(u8"좌하단", NULL, corner == 2)) corner = 2;
			if (ImGui::MenuItem(u8"우하단", NULL, corner == 3)) corner = 3;
			if (Open && ImGui::MenuItem(u8"종료"))*Open = false;
			ImGui::EndPopup();
		}
	}

	ImGui::End();
}
