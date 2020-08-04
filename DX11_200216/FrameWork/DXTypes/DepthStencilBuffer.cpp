#include "Framework.h"
#include "DepthStencilBuffer.h"

DepthStencilBuffer::DepthStencilBuffer()
	: texture2D(nullptr), DSV(nullptr), DSVReadOnly(nullptr), SRV(nullptr), SRVStencil(nullptr)
{
}

void DepthStencilBuffer::Init(UINT width, UINT Height, DXGI_FORMAT format, bool AsShaderResource, UINT multiSamples, UINT msQuality, UINT arraySize)
{
	this->Width = width;
	this->Height = Height;

	UINT BindFlags = D3D11_BIND_DEPTH_STENCIL;
	if (AsShaderResource == true)
		BindFlags |= D3D11_BIND_SHADER_RESOURCE;

	DXGI_FORMAT dsTexFormat;
	if (!AsShaderResource)
		dsTexFormat = format;
	else if (format == DXGI_FORMAT_D16_UNORM)
		dsTexFormat = DXGI_FORMAT_R16_TYPELESS;
	else if (format == DXGI_FORMAT_D24_UNORM_S8_UINT)
		dsTexFormat = DXGI_FORMAT_R24G8_TYPELESS;
	else
		dsTexFormat = DXGI_FORMAT_R32_TYPELESS;

	D3D11_TEXTURE2D_DESC Tex2DDesc;
	memset(&Tex2DDesc, 0, sizeof(Tex2DDesc));
	Tex2DDesc.ArraySize = arraySize;
	Tex2DDesc.Width = width;
	Tex2DDesc.Height = Height;
	Tex2DDesc.Format = dsTexFormat;
	Tex2DDesc.CPUAccessFlags = 0;
	Tex2DDesc.MipLevels = 1;
	Tex2DDesc.MiscFlags = 0;
	Tex2DDesc.BindFlags = BindFlags;
	Tex2DDesc.SampleDesc.Count = multiSamples;
	Tex2DDesc.SampleDesc.Quality = msQuality;
	Tex2DDesc.Usage = D3D11_USAGE_DEFAULT;

	Engine->GetDevice()->CreateTexture2D(&Tex2DDesc, nullptr, &texture2D);

	HRESULT hr = E_FAIL;

	DSVArray.clear();

	for (UINT i = 0; i < arraySize; ++i)	
	{
		ID3D11DepthStencilView* ArraySliceDSV = nullptr;

		D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
		dsvDesc.Format = format;
		dsvDesc.Flags = 0;

		if (arraySize == 1)
		{
			dsvDesc.Texture2D.MipSlice = 0;
			dsvDesc.ViewDimension = (multiSamples > 1) ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
		}

		else
		{
			dsvDesc.ViewDimension = (multiSamples > 1) ? D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY : D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
			dsvDesc.Texture2DArray.ArraySize = 1;
			dsvDesc.Texture2DArray.FirstArraySlice = i;
			dsvDesc.Texture2DArray.MipSlice = 0;
		}

		hr = Engine->GetDevice()->CreateDepthStencilView(texture2D, &dsvDesc, &ArraySliceDSV);
		DEBUG(hr);

		DSVArray.emplace_back(ArraySliceDSV);

		if (i == 0)
		{
			DSV = DSVArray[0];

			dsvDesc.Flags = D3D11_DSV_READ_ONLY_DEPTH;
			if (format == DXGI_FORMAT_D24_UNORM_S8_UINT || format == DXGI_FORMAT_D32_FLOAT_S8X24_UINT)
				dsvDesc.Flags |= D3D11_DSV_READ_ONLY_DEPTH;
			hr = Engine->GetDevice()->CreateDepthStencilView(texture2D, &dsvDesc, &DSVReadOnly);
			DEBUG(hr);
		}
	}

	if (AsShaderResource)
	{
		DXGI_FORMAT dsSRVFormat;
		if (format == DXGI_FORMAT_D16_UNORM)
			dsSRVFormat = DXGI_FORMAT_R16_UNORM;
		else if (format == DXGI_FORMAT_D24_UNORM_S8_UINT)
			dsSRVFormat = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		else
			dsSRVFormat = DXGI_FORMAT_R32_FLOAT;

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		srvDesc.Format = dsSRVFormat;

		if (arraySize == 1)
		{
			srvDesc.Texture2D.MipLevels = 1;
			srvDesc.Texture2D.MostDetailedMip = 0;
			srvDesc.ViewDimension = (multiSamples > 1) ? D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D;
		}
		else
		{
			srvDesc.ViewDimension = (multiSamples > 1) ? D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY : D3D11_SRV_DIMENSION_TEXTURE2DARRAY;

			srvDesc.Texture2DArray.ArraySize = arraySize;
			srvDesc.Texture2DArray.MipLevels = 1;
			srvDesc.Texture2DArray.MostDetailedMip = 0;
			srvDesc.Texture2DArray.FirstArraySlice = 0;
		}
		hr = Engine->GetDevice()->CreateShaderResourceView(texture2D, &srvDesc, &SRV);
		DEBUG(hr);

		SRVArray.clear();
		for (UINT i = 0; i < arraySize; ++i)
		{
			ID3D11ShaderResourceView* pSliceSRV = nullptr;

			if (arraySize == 1)
			{
				srvDesc.Texture2D.MipLevels = 1;
				srvDesc.Texture2D.MostDetailedMip = 0;
				srvDesc.ViewDimension = (multiSamples > 1) ? D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D;
			}
			else
			{
				srvDesc.ViewDimension = (multiSamples > 1) ? D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY : D3D11_SRV_DIMENSION_TEXTURE2DARRAY;

				srvDesc.Texture2DArray.ArraySize = 1;
				srvDesc.Texture2DArray.MipLevels = 1;
				srvDesc.Texture2DArray.MostDetailedMip = 0;
				srvDesc.Texture2DArray.FirstArraySlice = i;
			}

			hr = Engine->GetDevice()->CreateShaderResourceView(texture2D, &srvDesc, &pSliceSRV);
			DEBUG(hr);
			SRVArray.emplace_back(pSliceSRV);
		}
	}
	else
	{
		SRV = nullptr;
	}

	// Stencil Buffer»ý¼º
	if (format == DXGI_FORMAT_D24_UNORM_S8_UINT)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		srvDesc.Format = DXGI_FORMAT_X24_TYPELESS_G8_UINT;

		if (arraySize == 1)
		{
			srvDesc.Texture2D.MipLevels = 1;
			srvDesc.Texture2D.MostDetailedMip = 0;
			srvDesc.ViewDimension = (multiSamples > 1) ? D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D;
		}

		hr = Engine->GetDevice()->CreateShaderResourceView(texture2D, &srvDesc, &SRVStencil);
		DEBUG(hr);
	}
	else
	{
		SRVStencil = nullptr;
	}
}

void DepthStencilBuffer::Release()
{
	for (auto item : DSVArray)
		item->Release();

	for (auto item : SRVArray)
		item->Release();

	DSVReadOnly->Release();
	SRV->Release();
	SRVStencil->Release();
	texture2D->Release();
}
