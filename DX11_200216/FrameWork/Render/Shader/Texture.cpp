#include "Framework.h"
#include "Texture.h"

Texture::Texture(wstring Path, bool isSampler, D3D11_FILTER Filter, bool isDDS, D3D11_TEXTURE_ADDRESS_MODE Mode)
{
	if (!isDDS)
	{
		HRESULT hr = D3DX11CreateShaderResourceViewFromFile(Engine->GetDevice(), Path.c_str(), nullptr, nullptr, SRV.GetAddressOf(), nullptr);
		DEBUG(hr);
		if (SUCCEEDED(hr))
		{
			SRV->GetResource((ID3D11Resource**)srcTexture.GetAddressOf());
			srcTexture->GetDesc(&srcDesc);

			Width = srcDesc.Width;
			Height = srcDesc.Height;

			DEBUG(hr);
			if (isSampler)
			{
				D3D11_SAMPLER_DESC sampDesc = {};
				sampDesc.Filter = Filter;
				sampDesc.AddressU = Mode;
				sampDesc.AddressV = Mode;
				sampDesc.AddressW = Mode;
				sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
				sampDesc.MinLOD = 0;
				sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

				hr = Engine->GetDevice()->CreateSamplerState(&sampDesc, Sampler.GetAddressOf());
				DEBUG(hr);
			}
		}
	}
	else
	{
		HRESULT hr = DirectX::CreateDDSTextureFromFile(Engine->GetDevice(), Engine->GetContext(), Path.c_str(), (ID3D11Resource**)srcTexture.GetAddressOf(), SRV.GetAddressOf());
		srcTexture->GetDesc(&srcDesc);

		Width = srcDesc.Width;
		Height = srcDesc.Height;
		DEBUG(hr);
	}
}

Texture::Texture()
	: Width(0), Height(0)
{
}

Texture::~Texture()
{
}

void Texture::PSSet(UINT slot)
{
	Engine->GetContext()->PSSetShaderResources(slot, 1, SRV.GetAddressOf());

	if (Sampler != nullptr)
	{
		Engine->GetContext()->PSSetSamplers(slot, 1, Sampler.GetAddressOf());
	}
}

void Texture::CSSet(UINT slot)
{
	Engine->GetContext()->CSSetShaderResources(slot, 1, SRV.GetAddressOf());

	if (Sampler != nullptr)
	{
		Engine->GetContext()->CSSetSamplers(slot, 1, Sampler.GetAddressOf());
	}
}

void Texture::Clear(UINT slot)
{
	ID3D11ShaderResourceView* srv = nullptr;
	Engine->GetContext()->PSSetShaderResources(slot, 1, &srv);
	Engine->GetContext()->VSSetShaderResources(slot, 1, &srv);
	Engine->GetContext()->CSSetShaderResources(slot, 1, &srv);
}

ID3D11ShaderResourceView* Texture::GetResource()
{
	return SRV.Get();
}

void Texture::CreateRandomTexture(int Width, int Height, DXGI_FORMAT format)
{
	this->Width = Width;
	this->Height = Height;
	float* randompixel = new float[Width * Height];
	default_random_engine generator;

	for (UINT i = 0; i < Width * Height; i++)
	{
		uniform_real_distribution<float> distributation(0, 1);
		randompixel[i] = distributation(generator);
	}

	ZeroMemory(&srcDesc, sizeof(D3D11_TEXTURE2D_DESC));
	srcDesc.Width = Width;
	srcDesc.Height = Height;
	srcDesc.MipLevels = 1;
	srcDesc.ArraySize = 1;
	srcDesc.Format = format;
	srcDesc.SampleDesc.Count = 1;
	srcDesc.Usage = D3D11_USAGE_DYNAMIC;
	srcDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	srcDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA subresource;
	subresource.pSysMem = randompixel;
	subresource.SysMemPitch = srcDesc.Width * 4;
	subresource.SysMemSlicePitch = 0;

	HRESULT hr = Engine->GetDevice()->CreateTexture2D(&srcDesc, &subresource, srcTexture.GetAddressOf());

	DEBUG(hr);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	srvDesc.Format = format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = srcDesc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;

	hr = Engine->GetDevice()->CreateShaderResourceView(srcTexture.Get(), &srvDesc, SRV.GetAddressOf());

	DEBUG(hr);

	delete[] randompixel;

	D3D11_SAMPLER_DESC sampDesc = {};
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	hr = Engine->GetDevice()->CreateSamplerState(&sampDesc, Sampler.GetAddressOf());
	DEBUG(hr);
}

UINT Texture::GetWidth()
{
	return Width;
}

UINT Texture::GetHeight()
{
	return Height;
}
