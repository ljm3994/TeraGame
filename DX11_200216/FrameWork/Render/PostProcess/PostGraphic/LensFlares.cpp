#include "Framework.h"
#include "LensFlares.h"
#include "FrameWork\Render\PostProcess\FullScreenPass.h"

LensFlares::LensFlares()
{
}

LensFlares::~LensFlares()
{
}

void LensFlares::Init(FullScreenPass* FullscreenPass, UINT FullscreenWidth, UINT FullscreenHeight)
{
	this->fullscreenpass = FullscreenPass;

	renderTargetHalfRes.Init(FullscreenWidth / 4, FullscreenHeight / 4, DXGI_FORMAT_R11G11B10_FLOAT);
	renderTargetHalfRes2.Init(FullscreenWidth / 4, FullscreenHeight / 4, DXGI_FORMAT_R11G11B10_FLOAT);

	PSShaderThresHold = Engine->AddShader("LensFlaresThreshold", L"LensFlares", ShaderCreateFlag::SHADER_FLAG_PS, "", "LensFlaresThreshold");
	PSShaderGhost = Engine->AddShader("LensFlaresGhost", L"LensFlares", ShaderCreateFlag::SHADER_FLAG_PS, "", "LensFlaresGhosts");
	PSShaderBlurH = Engine->AddShader("LensFlaresBlurH", L"LensFlaresBlur", ShaderCreateFlag::SHADER_FLAG_PS, "", "LensFlaresBlurH");
	PSShaderBlurV = Engine->AddShader("LensFlaresBlurV", L"LensFlaresBlur", ShaderCreateFlag::SHADER_FLAG_PS, "", "LensFlaresBlurV");
}

void LensFlares::Render(ID3D11ShaderResourceView* InputSRV)
{
	ID3D11RenderTargetView* pOutputs[1] = { nullptr };
	Engine->GetContext()->OMSetRenderTargets(1, pOutputs, nullptr);

	ID3D11ShaderResourceView* pInputs[1] = { InputSRV };
	Engine->GetContext()->PSSetShaderResources(3, 1, pInputs);

	pOutputs[0] = { renderTargetHalfRes.rtv };
	Engine->GetContext()->OMSetRenderTargets(1, pOutputs, nullptr);

	//PSShaderThresHold->SetPS();
	//
	//fullscreenpass->Render(renderTargetHalfRes.Width, renderTargetHalfRes.Height);

	pOutputs[0] = nullptr;
	Engine->GetContext()->OMSetRenderTargets(1, pOutputs, nullptr);

	pInputs[0] = InputSRV;//renderTargetHalfRes.srv;
	Engine->GetContext()->PSSetShaderResources(3, 1, pInputs);

	pOutputs[0] = renderTargetHalfRes2.rtv;
	Engine->GetContext()->OMSetRenderTargets(1, pOutputs, nullptr);

	PSShaderGhost->SetPS();
	fullscreenpass->Render(renderTargetHalfRes2.Width, renderTargetHalfRes2.Height);

	RenderTarget2D* pTextures[2] = { &renderTargetHalfRes, &renderTargetHalfRes2 };

	const UINT TextureWidth = renderTargetHalfRes2.Width;
	const UINT TextureHeight = renderTargetHalfRes2.Height;

	for (UINT i = 0; i < 4; i++)
	{
		pInputs[0] = nullptr;
		Engine->GetContext()->PSSetShaderResources(3, 1, pInputs);

		fullscreenpass->Render(TextureWidth, TextureHeight, renderTargetHalfRes2.srv, renderTargetHalfRes.rtv, PSShaderBlurH);

		Engine->GetContext()->PSSetShaderResources(3, 1, pInputs);

		fullscreenpass->Render(TextureWidth, TextureHeight, renderTargetHalfRes.srv, renderTargetHalfRes2.rtv, PSShaderBlurV);
	}

	pOutputs[0] = nullptr;
	Engine->GetContext()->OMSetRenderTargets(1, pOutputs, nullptr);
}

void LensFlares::Release()
{
	renderTargetHalfRes2.Release();
	renderTargetHalfRes.Release();
}
