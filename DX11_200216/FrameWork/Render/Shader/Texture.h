#pragma once

class Texture
{
private:
	friend class DXEngine;
	ComPtr<ID3D11ShaderResourceView> SRV;
	ComPtr<ID3D11SamplerState> Sampler;

	UINT Width, Height;

	ComPtr<ID3D11Texture2D> srcTexture;
	D3D11_TEXTURE2D_DESC srcDesc;
	Texture(wstring Path, bool isSampler, D3D11_FILTER Filter, bool isDDS, D3D11_TEXTURE_ADDRESS_MODE Mode);
	Texture();
public:
	~Texture();
	void PSSet(UINT slot);
	void CSSet(UINT slot);
	void Clear(UINT slot);
	ID3D11ShaderResourceView* GetResource();
	void CreateRandomTexture(int Width, int Height, DXGI_FORMAT format = DXGI_FORMAT_R32_FLOAT);
	UINT GetWidth();
	UINT GetHeight();
	ID3D11ShaderResourceView * GetSrv() { return SRV.Get(); }
};