#include "Framework.h"
#include "BlendState.h"

BlendState::BlendState()
	: State(nullptr)
{
	ZeroMemory(&desc, sizeof(D3D11_BLEND_DESC));

	desc.AlphaToCoverageEnable = false;
	desc.IndependentBlendEnable = false;

	desc.RenderTarget[0].BlendEnable = false;
	desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

	desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
	desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

	desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	Change();
}

BlendState::~BlendState()
{
}

void BlendState::Set(D3DXVECTOR4 blendFactor)
{
	Engine->GetContext()->OMSetBlendState(State.Get(), blendFactor, 0xffffffff);
}

void BlendState::Alpha(bool val)
{
	desc.RenderTarget[0].BlendEnable = val;
	desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

	Change();
}

void BlendState::RenderTargetSet(bool val, D3D11_BLEND srcBlend, D3D11_BLEND DestBlend, D3D11_BLEND_OP BlendOP, D3D11_BLEND_OP BlendOpAlpha, D3D11_BLEND srcBlendAlpha, D3D11_BLEND DestBlendAlpha, D3D11_COLOR_WRITE_ENABLE RenderTargetWriteMask)
{
	desc.RenderTarget[0].BlendEnable = val;
	desc.RenderTarget[0].SrcBlend = srcBlend;
	desc.RenderTarget[0].DestBlend = DestBlend;
	desc.RenderTarget[0].BlendOp = BlendOP;
	desc.RenderTarget[0].BlendOpAlpha = BlendOpAlpha;

	desc.RenderTarget[0].SrcBlendAlpha = srcBlendAlpha;
	desc.RenderTarget[0].DestBlendAlpha = DestBlendAlpha;
	desc.RenderTarget[0].RenderTargetWriteMask = RenderTargetWriteMask;

	Change();
}

void BlendState::AlphatoCoberage(bool val)
{
	desc.AlphaToCoverageEnable = val;

	Change();
}

void BlendState::Additive()
{
	desc.RenderTarget[0].BlendEnable = true;
	desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

	Change();
}

void BlendState::BlendOP(D3D11_BLEND_OP val)
{
	desc.RenderTarget[0].BlendOp = val;

	Change();
}

void BlendState::DestBlend(D3D11_BLEND val)
{
	desc.RenderTarget[0].DestBlend = val;

	Change();
}

void BlendState::SrcBlend(D3D11_BLEND val)
{
	desc.RenderTarget[0].SrcBlend = val;

	Change();
}

void BlendState::Change()
{
	HRESULT hr = Engine->GetDevice()->CreateBlendState(&desc, State.GetAddressOf());
	DEBUG(hr);
}
