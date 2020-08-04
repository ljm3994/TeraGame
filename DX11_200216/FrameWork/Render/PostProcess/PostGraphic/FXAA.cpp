#include "Framework.h"
#include "FXAA.h"

FXAA::FXAA()
	: PostProcess()
{
}

FXAA::~FXAA()
{
}

void FXAA::Init()
{
	PostProcess::Init();
	shaderPS = Engine->AddShader("FXAA", L"FXAA", ShaderCreateFlag::SHADER_FLAG_PS);
}

void FXAA::Render(ID3D11ShaderResourceView* input, ID3D11RenderTargetView* output)
{
	PostProcess::Render(input, output);

	Process(input, output, shaderPS);
}

void FXAA::PostRender(UINT width, UINT height)
{
	PostProcess::PostRender(width, height);
}
