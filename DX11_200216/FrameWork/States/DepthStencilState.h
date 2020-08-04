#pragma once

class DepthStencilState
{
private:
	D3D11_DEPTH_STENCIL_DESC desc;
	ComPtr<ID3D11DepthStencilState> state;

public:
	DepthStencilState();
	~DepthStencilState();

	void Set(UINT stencilRef = 1);
	void DepthFunc(D3D11_COMPARISON_FUNC func);
	void StencilEnable(bool val);
	void DepthEnable(bool val);
	void DepthWirteMask(D3D11_DEPTH_WRITE_MASK val);

	void Changed();
};