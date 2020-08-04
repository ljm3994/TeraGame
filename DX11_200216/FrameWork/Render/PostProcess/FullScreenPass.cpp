#include "Framework.h"
#include "FullScreenPass.h"

FullScreenPass::FullScreenPass()
{
}

FullScreenPass::~FullScreenPass()
{
}

void FullScreenPass::Create()
{
	shader = Engine->AddShader("FullScreenPass", L"FullScreenPass", ShaderCreateFlag::SHADER_FLAG_PS);
	RecopyShader = Engine->AddShader("PostProcessCommon", L"PostProcessCommon");
}

void FullScreenPass::RenderTri()
{
	RecopyShader->SetVS();

	Engine->GetContext()->Draw(3, 0);
}

void FullScreenPass::Render(UINT width, UINT height)
{
	D3D11_VIEWPORT vpOld[1];
	UINT nViewPorts = 1;
	Engine->GetContext()->RSGetViewports(&nViewPorts, vpOld);

	D3D11_VIEWPORT vp;
	vp.Width = static_cast<float>(width);
	vp.Height = static_cast<float>(height);
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;

	Engine->GetContext()->RSSetViewports(1, &vp);

	RenderTri();

	Engine->GetContext()->RSSetViewports(nViewPorts, vpOld);
}

void FullScreenPass::Render(UINT targetWidth, UINT targetHeight, ID3D11ShaderResourceView* InSourceSRV, ID3D11RenderTargetView* OutTargetRTV, Shader* PixelShader)
{
	D3D11_VIEWPORT vpOld[1];
	UINT nViewPorts = 1;
	Engine->GetContext()->RSGetViewports(&nViewPorts, vpOld);

	D3D11_VIEWPORT vp;
	vp.Width = static_cast<float>(targetWidth);
	vp.Height = static_cast<float>(targetHeight);
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	Engine->GetContext()->RSSetViewports(1, &vp);

	ID3D11RenderTargetView* renderTargets[1] = { OutTargetRTV };
	Engine->GetContext()->OMSetRenderTargets(1, renderTargets, nullptr);

	ID3D11ShaderResourceView* shaderResources[1] = { InSourceSRV };
	Engine->GetContext()->PSSetShaderResources(3, 1, shaderResources);

	PixelShader->SetPS();

	D3DXVECTOR4& cbufferData = buffer.GetData();
	cbufferData.x = vp.Width;
	cbufferData.y = vp.Height;
	cbufferData.z = 0.0f;
	buffer.SetPSBuffer(6);

	RenderTri();

	Engine->GetContext()->RSSetViewports(nViewPorts, vpOld);
}

void FullScreenPass::RenderTriEx(ID3D11RenderTargetView* OutRTV, ID3D11ShaderResourceView* InSRV, Shader* OverridePS)
{
	ID3D11RenderTargetView* renderTargets[1] = { OutRTV };
	Engine->GetContext()->OMSetRenderTargets(1, renderTargets, nullptr);

	RecopyShader->SetVS();

	ID3D11ShaderResourceView* shaderResources[1] = { InSRV };
	Engine->GetContext()->PSSetShaderResources(3, 1, shaderResources);

	OverridePS->SetPS();

	Engine->GetContext()->Draw(3, 0);
}

void FullScreenPass::RenderFullScreenTriRecopy(ID3D11RenderTargetView* OutRTV, ID3D11ShaderResourceView* InSRV)
{
	ID3D11RenderTargetView* renderTargets[1] = { OutRTV };
	Engine->GetContext()->OMSetRenderTargets(1, renderTargets, nullptr);

	RecopyShader->SetVS();

	ID3D11ShaderResourceView* shaderResources[1] = { InSRV };
	Engine->GetContext()->PSSetShaderResources(3, 1, shaderResources);
	
	RecopyShader->SetPS();

	Engine->GetContext()->Draw(3, 0);
}

void FullScreenPass::RenderScaling(UINT targetWidth, UINT targetHeight, ID3D11ShaderResourceView* InSourceSRV, ID3D11RenderTargetView* OutTargetRTV)
{
	Render(targetWidth, targetHeight, InSourceSRV, OutTargetRTV, RecopyShader);
}
