#include "Framework.h"
#include "Sharpen.h"
#include "FrameWork\Render\PostProcess\FullScreenPass.h"
Sharpen::Sharpen()
{
}

Sharpen::~Sharpen()
{
}

void Sharpen::Init(FullScreenPass* fullscreenpass)
{
	this->fullscreenpass = fullscreenpass;

	shader = Engine->AddShader("Sharpen", L"Sharpen", ShaderCreateFlag::SHADER_FLAG_PS);
}

void Sharpen::Render(ID3D11ShaderResourceView* Inputs[], UINT InputsCount, ID3D11RenderTargetView* Output)
{
	ID3D11RenderTargetView* target[1] = { Output };

	Engine->GetContext()->OMSetRenderTargets(1, target, nullptr);

	sharpenbuffer.SetPSBuffer(5);

	Engine->GetContext()->PSSetShaderResources(0, InputsCount, Inputs);
	shader->SetPS();

	fullscreenpass->RenderTri();
}

