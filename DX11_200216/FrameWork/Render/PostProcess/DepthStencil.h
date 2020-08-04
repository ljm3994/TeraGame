#pragma once

class DepthStencil
{
private:
	ComPtr< ID3D11Texture2D> dsvTexture;
	ComPtr< ID3D11DepthStencilView> dsv;
	ComPtr< ID3D11ShaderResourceView> srv;

public:
	DepthStencil(UINT Width, UINT Height);
	~DepthStencil();

	ID3D11DepthStencilView* GetDSV() { return dsv.Get(); }
	ID3D11ShaderResourceView* GetSRV() { return srv.Get(); }
};