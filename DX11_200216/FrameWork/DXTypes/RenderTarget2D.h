#pragma once

class RenderTarget2D
{
public:
	ID3D11Texture2D* texture;
	ID3D11RenderTargetView* rtv;
	ID3D11ShaderResourceView* srv;
	ID3D11UnorderedAccessView* uav;

	UINT Width;
	UINT Height;

	RenderTarget2D();
	void Init(UINT width, UINT Height, DXGI_FORMAT format, bool buseuav = false, bool bgenerateMip = false, UINT numMipLevels = 1);

	void Release();

	operator ID3D11RenderTargetView* ()		const { return rtv; }
	operator ID3D11ShaderResourceView* ()	const { return srv; }
	operator ID3D11UnorderedAccessView* ()	const { return uav; }


};