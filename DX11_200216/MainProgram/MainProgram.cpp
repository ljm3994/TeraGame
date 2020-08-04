#include "Framework.h"
#include "MainProgram.h"
#include "Scene/TerrianEditorScene.h"
#include "Scene/TestScene.h"
#include "Scene\LoadingScene.h"
#include "Scene\IntroScene.h"
MainProgram::MainProgram()
{
	Engine->AddScene("Test", new TestScene());
	Engine->AddScene("Intro", new IntroScene());
	Engine->AddScene("Loading", new LoadingScene());
	Engine->ChangeScene("Intro");
}

MainProgram::~MainProgram()
{

}

void MainProgram::Init()
{
	SetImgui();
}

void MainProgram::Update(ID3D11RenderTargetView* backbuffer, UINT ClienWidth, UINT ClientHeight, float delta, float TotalTime)
{
	Engine->Update(backbuffer, ClienWidth, ClientHeight, delta, TotalTime);
}

void MainProgram::PreRender()
{
	Engine->PreRender();
}

void MainProgram::Render()
{
	Engine->Render();
}

void MainProgram::PostRender()
{
	Engine->PostRender();
}

void MainProgram::SetImgui()
{
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontFromFileTTF("Font/BareunDotumPro3.ttf", 18.0F, NULL, io.Fonts->GetGlyphRangesKorean());
}
