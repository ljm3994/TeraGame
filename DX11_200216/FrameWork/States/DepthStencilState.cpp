#include "Framework.h"
#include "DepthStencilState.h"

DepthStencilState::DepthStencilState()
	: state(nullptr)
{
	ZeroMemory(&desc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	desc.DepthEnable = true;
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	desc.DepthFunc = D3D11_COMPARISON_LESS;

	desc.StencilEnable = true;
	desc.StencilReadMask = 0xff;
	desc.StencilWriteMask = 0xff;

	desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	Changed();
}

DepthStencilState::~DepthStencilState()
{
}

void DepthStencilState::Set(UINT stencilRef)
{
	Engine->GetContext()->OMSetDepthStencilState(state.Get(), stencilRef);
}

void DepthStencilState::DepthFunc(D3D11_COMPARISON_FUNC func)
{
	desc.DepthFunc = func;

	Changed();
}

void DepthStencilState::StencilEnable(bool val)
{
	desc.StencilEnable = val;

	Changed();
}

void DepthStencilState::DepthEnable(bool val)
{
	desc.DepthEnable = val;

	Changed();
}

void DepthStencilState::DepthWirteMask(D3D11_DEPTH_WRITE_MASK val)
{
	desc.DepthWriteMask = val;

	Changed();
}

void DepthStencilState::Changed()
{
	if (state != nullptr)
		state.Reset();

	HRESULT hr = Engine->GetDevice()->CreateDepthStencilState(&desc, state.GetAddressOf());
	DEBUG(hr);
}
