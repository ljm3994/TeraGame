#pragma once

class DepthStencilBuffer
{
public:
	vector<ID3D11DepthStencilView*> DSVArray;
	vector<ID3D11ShaderResourceView*> SRVArray;

	ID3D11Texture2D* texture2D;
	ID3D11DepthStencilView* DSV;
	ID3D11DepthStencilView* DSVReadOnly;
	ID3D11ShaderResourceView* SRV;
	ID3D11ShaderResourceView* SRVStencil;

	UINT Width;
	UINT Height;
	
	DepthStencilBuffer();

	void Init(UINT width, UINT Height, DXGI_FORMAT format = DXGI_FORMAT_D24_UNORM_S8_UINT,
		bool AsShaderResource = false, UINT multiSamples = 1, UINT msQuality = 0, UINT arraySize = 1);

	void Release();

	operator ID3D11ShaderResourceView* () const { return SRV; }
	operator ID3D11DepthStencilView* () const { return DSVArray[0]; }
};