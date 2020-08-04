#include "Framework.h"
#include "ShadowMap.h"

ShadowMap::ShadowMap()
{
}

void ShadowMap::Init(UINT width, UINT height, DXGI_FORMAT format)
{
	this->width = width;
	this->height = height;

	ShadowMapViewport.TopLeftX = 0.f;
	ShadowMapViewport.TopLeftY = 0.f;
	ShadowMapViewport.Width = static_cast<float>(width);
	ShadowMapViewport.Height = static_cast<float>(height);
	ShadowMapViewport.MinDepth = 0.f;
	ShadowMapViewport.MaxDepth = 1.f;

	DXGI_FORMAT dsTexFormat;
	if (format == DXGI_FORMAT_D16_UNORM)
		dsTexFormat = DXGI_FORMAT_R16_TYPELESS;
	else if (format == DXGI_FORMAT_D24_UNORM_S8_UINT)
		dsTexFormat = DXGI_FORMAT_R24G8_TYPELESS;
	else
		dsTexFormat = DXGI_FORMAT_R32_TYPELESS;

	UINT BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;

	D3D11_TEXTURE2D_DESC Tex2DDesc;
	memset(&Tex2DDesc, 0, sizeof(Tex2DDesc));
	Tex2DDesc.ArraySize = 1;
	Tex2DDesc.Width = width;
	Tex2DDesc.Height = height;
	Tex2DDesc.Format = dsTexFormat;
	Tex2DDesc.CPUAccessFlags = 0;
	Tex2DDesc.MipLevels = 1;
	Tex2DDesc.MiscFlags = 0;
	Tex2DDesc.BindFlags = BindFlags;
	Tex2DDesc.SampleDesc.Count = 1;
	Tex2DDesc.SampleDesc.Quality = 0;
	Tex2DDesc.Usage = D3D11_USAGE_DEFAULT;

	HRESULT hr = Engine->GetDevice()->CreateTexture2D(&Tex2DDesc, nullptr, &texture2D);
	DEBUG(hr);

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Format = format;
	dsvDesc.Flags = 0;
	dsvDesc.Texture2D.MipSlice = 0;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

	hr = Engine->GetDevice()->CreateDepthStencilView(texture2D, &dsvDesc, &DSV);

	DXGI_FORMAT dsSRVFormat;
	if (format == DXGI_FORMAT_D16_UNORM)
		dsSRVFormat = DXGI_FORMAT_R16_UNORM;
	else if (format == DXGI_FORMAT_D24_UNORM_S8_UINT)
		dsSRVFormat = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	else
		dsSRVFormat = DXGI_FORMAT_R32_FLOAT;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = dsSRVFormat;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

	hr = Engine->GetDevice()->CreateShaderResourceView(texture2D, &srvDesc, &SRV);
	DEBUG(hr);
}

void ShadowMap::Release()
{
	SRV->Release();
	DSV->Release();
	texture2D->Release();
}
