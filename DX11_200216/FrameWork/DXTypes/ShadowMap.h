#pragma once

class ShadowMap
{
public:
	ID3D11Texture2D* texture2D;
	ID3D11DepthStencilView* DSV;
	ID3D11ShaderResourceView* SRV;

	D3D11_VIEWPORT				ShadowMapViewport;

	UINT						width;
	UINT						height;

	ShadowMap();

	void Init(UINT width, UINT height, DXGI_FORMAT format = DXGI_FORMAT_D24_UNORM_S8_UINT);

	void Release();
};