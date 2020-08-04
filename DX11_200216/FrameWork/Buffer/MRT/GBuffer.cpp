#include "Framework.h"
#include "GBuffer.h"

void GBuffer::Init(UINT width, UINT height)
{
	DXGI_FORMAT fmtDepthStencil = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthstencilbuffer.Init(width, height, fmtDepthStencil, true);

	RenderTarget0.Init(width, height, DXGI_FORMAT_R8G8B8A8_UNORM);
	RenderTarget1.Init(width, height, DXGI_FORMAT_R8G8B8A8_UNORM);
	RenderTarget2.Init(width, height, DXGI_FORMAT_R8G8B8A8_UNORM);

	RenderTarget3.Init(width, height, DXGI_FORMAT_R16G16_FLOAT);
}

void GBuffer::Set()
{
	ID3D11RenderTargetView* GBufferRTVs[4] =
	{
		RenderTarget0.rtv,
		RenderTarget1.rtv,
		RenderTarget2.rtv,
		RenderTarget3.rtv
	};

	Engine->GetContext()->OMSetRenderTargets(4, GBufferRTVs, depthstencilbuffer.DSV);
}

void GBuffer::ClearGBuffer(float DepthClearValue, bool bClearGBuffer)
{
	Engine->GetContext()->ClearDepthStencilView(depthstencilbuffer.DSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, DepthClearValue, 0);

	const float ClearColor[4] = { 0.f, 0.f, 0.f, 0.f };
	if (bClearGBuffer)
	{
		Engine->GetContext()->ClearRenderTargetView(RenderTarget0.rtv, ClearColor);
		Engine->GetContext()->ClearRenderTargetView(RenderTarget1.rtv, ClearColor);
		Engine->GetContext()->ClearRenderTargetView(RenderTarget2.rtv, ClearColor);
	}

	// motion blur
	Engine->GetContext()->ClearRenderTargetView(RenderTarget3.rtv, ClearColor);
}

void GBuffer::Release()
{
	depthstencilbuffer.Release();

	RenderTarget0.Release();
	RenderTarget1.Release();
	RenderTarget2.Release();
	RenderTarget3.Release();
}
