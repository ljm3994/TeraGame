#include "Framework.h"
#include "RasterizerState.h"

RasterizeState::RasterizeState()
	: state(nullptr)
{
	ZeroMemory(&desc, sizeof(D3D11_RASTERIZER_DESC));
	desc.AntialiasedLineEnable = false;
	desc.CullMode = D3D11_CULL_BACK;
	desc.FillMode = D3D11_FILL_SOLID;

	Changed();
}

RasterizeState::~RasterizeState()
{
}

void RasterizeState::Set()
{
	Engine->GetContext()->RSSetState(state.Get());
}

void RasterizeState::FillMode(D3D11_FILL_MODE mode)
{
	desc.FillMode = mode;
	Changed();
}

void RasterizeState::CullMode(D3D11_CULL_MODE mode)
{
	desc.CullMode = mode;

	Changed();
}

void RasterizeState::FrontCounterClockwise(bool val)
{
	desc.FrontCounterClockwise = val;
	Changed();
}

void RasterizeState::Changed()
{
	if (state != nullptr)
		state.Reset();

	HRESULT hr = Engine->GetDevice()->CreateRasterizerState(&desc, state.GetAddressOf());
	DEBUG(hr);
}
