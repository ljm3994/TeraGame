#include "Framework.h"
#include "RenderTarget2D.h"

RenderTarget2D::RenderTarget2D()
	: texture(nullptr)
	, rtv(nullptr)
	, srv(nullptr)
	, uav(nullptr)
{
}

void RenderTarget2D::Init(UINT width, UINT Height, DXGI_FORMAT format, bool buseuav, bool bgenerateMip, UINT numMipLevels)
{
	this->Width = width;
	this->Height = Height;

	UINT bindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	if (buseuav)
	{
		bindFlags |= D3D11_BIND_UNORDERED_ACCESS;
	}

	UINT miscFlags = 0;
	if (bgenerateMip && numMipLevels != 1)
	{
		miscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;
	}

	D3D11_TEXTURE2D_DESC tex2dDesc;
	memset(&tex2dDesc, 0, sizeof(tex2dDesc));

	tex2dDesc.ArraySize = 1;
	tex2dDesc.BindFlags = bindFlags;
	tex2dDesc.Usage = D3D11_USAGE_DEFAULT;
	tex2dDesc.Format = format;
	tex2dDesc.Width = Width;
	tex2dDesc.Height = Height;
	tex2dDesc.MipLevels = numMipLevels;
	tex2dDesc.SampleDesc.Count = 1;
	tex2dDesc.MiscFlags = miscFlags;

	HRESULT hr;
	hr = Engine->GetDevice()->CreateTexture2D(&tex2dDesc, nullptr, &texture);
	DEBUG(hr);

	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	rtvDesc.Format = tex2dDesc.Format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;

	hr = Engine->GetDevice()->CreateRenderTargetView(texture, &rtvDesc, &rtv);
	DEBUG(hr);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = tex2dDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = -1;
	srvDesc.Texture2D.MostDetailedMip = 0;

	hr = Engine->GetDevice()->CreateShaderResourceView(texture, &srvDesc, &srv);
	DEBUG(hr);
	if (buseuav)
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
		uavDesc.Format = tex2dDesc.Format;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
		uavDesc.Texture2D.MipSlice = 0;

		hr = Engine->GetDevice()->CreateUnorderedAccessView(texture, &uavDesc, &uav);
		DEBUG(hr);
	}
}

void RenderTarget2D::Release()
{
	srv->Release();
	rtv->Release();
	texture->Release();
}
