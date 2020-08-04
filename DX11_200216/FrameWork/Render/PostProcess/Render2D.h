#pragma once

class Render2D
{
	D3D11_VIEWPORT vp;

	UINT width, height;

	ComPtr< ID3D11ShaderResourceView> srv;
	ComPtr< ID3D11Texture2D> rtvTexture;
	ComPtr< ID3D11RenderTargetView> rtv;

	ComPtr< ID3D11Texture2D> dsvTexture;
	ComPtr< ID3D11DepthStencilView> dsv;

public:
	Render2D(UINT Width, UINT height);
	~Render2D();

	void Set(D3DXCOLOR color = D3DXCOLOR(1, 1, 1, 1));

	ID3D11ShaderResourceView* GetSRV() { return srv.Get(); }
	ID3D11RenderTargetView* GetRTV() { return rtv.Get(); }
	ID3D11DepthStencilView* GetDSV() { return dsv.Get(); }
};