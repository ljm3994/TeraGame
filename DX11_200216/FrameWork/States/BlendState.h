#pragma once

class BlendState
{
	ComPtr<ID3D11BlendState> State;
	D3D11_BLEND_DESC desc;
public:
	BlendState();
	~BlendState();

	void Set(D3DXVECTOR4 blendFactor = { 0.0f, 0.0f, 0.0f, 0.0f});

	void Alpha(bool val);
	void RenderTargetSet(bool val, D3D11_BLEND srcBlend, D3D11_BLEND DestBlend, D3D11_BLEND_OP BlendOP, D3D11_BLEND_OP BlendOpAlpha, D3D11_BLEND srcBlendAlpha, D3D11_BLEND DestBlendAlpha, D3D11_COLOR_WRITE_ENABLE RenderTargetWriteMask);
	void AlphatoCoberage(bool val);
	void Additive();
	void BlendOP(D3D11_BLEND_OP val);
	void DestBlend(D3D11_BLEND val);
	void SrcBlend(D3D11_BLEND val);

	void Change();
};