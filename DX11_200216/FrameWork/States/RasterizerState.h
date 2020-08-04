#pragma once

class RasterizeState
{
private:
	D3D11_RASTERIZER_DESC desc;
	ComPtr<ID3D11RasterizerState> state;
public:
	RasterizeState();
	~RasterizeState();

	void Set();

	void FillMode(D3D11_FILL_MODE mode);
	void CullMode(D3D11_CULL_MODE mode);
	void FrontCounterClockwise(bool val);
	D3D11_RASTERIZER_DESC& GetDesc() { return desc; }

	void Changed();
};