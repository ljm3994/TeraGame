#include "Framework.h"
#include "PostProcess.h"

PostProcess::PostProcess()
{
}

PostProcess::~PostProcess()
{
	ClearRenderTargetCache();
	delete postbuffer;
}

void PostProcess::Init()
{
	postbuffer = new PostProcesPS();
	shader = Engine->AddShader("PostPrcessCommon", L"PostProcessCommon", ShaderCreateFlag::SHADER_FLAG_VS);
}

void PostProcess::Render(ID3D11ShaderResourceView* input, ID3D11RenderTargetView* output)
{
	shader->SetVS();
}

void PostProcess::PostRender(UINT width, UINT height)
{
	ClearRenderTargetCache();

	inputWidth = width;
	inputHeight = height;
}

RenderTarget* PostProcess::GetRenderTarget(UINT width, UINT height, DXGI_FORMAT format, UINT msCount, UINT msQuality, UINT mipLevels, bool generateMipMaps, bool useAsUAV)
{
	for (size_t i = 0; i < rendertargets.size(); ++i)
	{
		RenderTarget* tr = rendertargets[i];
		if (!tr->bInuse && tr->width == width && tr->height == height && tr->format == format && tr->MSCount == msCount &&
			tr->MSQuality == msQuality && (tr->uav != nullptr) == useAsUAV)
		{
			tr->bInuse = true;
			return tr;
		}
	}

	RenderTarget* rt = new RenderTarget();
	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = width;
	desc.Height = height;
	desc.ArraySize = 1;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	if (useAsUAV)
		desc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;
	desc.CPUAccessFlags = 0;
	desc.Format = format;
	desc.MipLevels = mipLevels;
	desc.MiscFlags = generateMipMaps ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;
	desc.SampleDesc.Count = msCount;
	desc.SampleDesc.Quality = msQuality;
	desc.Usage = D3D11_USAGE_DEFAULT;

	HRESULT hr;

	hr = Engine->GetDevice()->CreateTexture2D(&desc, nullptr, rt->Texture.GetAddressOf());
	DEBUG(hr);
	hr = Engine->GetDevice()->CreateRenderTargetView(rt->Texture.Get(), nullptr, rt->rtv.GetAddressOf());
	DEBUG(hr);
	hr = Engine->GetDevice()->CreateShaderResourceView(rt->Texture.Get(), nullptr, rt->srv.GetAddressOf());
	DEBUG(hr);

	if (useAsUAV)
		hr = Engine->GetDevice()->CreateUnorderedAccessView(rt->Texture.Get(), nullptr, rt->uav.GetAddressOf());
	else
		rt->uav = nullptr;

	rt->width = width;
	rt->height = height;
	rt->MSCount = msCount;
	rt->MSQuality = msQuality;
	rt->format = format;
	rt->bInuse = true;
	rendertargets.emplace_back(rt);

	return rendertargets.back();
}

void PostProcess::ClearRenderTargetCache()
{
	for (auto item : rendertargets)
	{
		item->Texture.Reset();
		item->rtv.Reset();
		item->srv.Reset();
		item->uav.Reset();

		delete item;
	}

	rendertargets.clear();
	rendertargets.shrink_to_fit();
}

void PostProcess::Process(ID3D11ShaderResourceView* input, ID3D11RenderTargetView* output, Shader* Shader)
{
	inputs.emplace_back(input);
	outputs.emplace_back(output);

	Process(Shader);
}

void PostProcess::Process(Shader* Shader)
{
	ID3D11RenderTargetView** rtv = reinterpret_cast<ID3D11RenderTargetView**>(&outputs[0]);
	UINT numRtv = (UINT)outputs.size();
	
	Engine->GetContext()->OMSetRenderTargets(numRtv, rtv, nullptr);

	static const float ClearColor[4] = { 0.f, 0.f, 0.f, 0.f };

	Engine->GetContext()->ClearRenderTargetView(*rtv, ClearColor);

	ID3D11ShaderResourceView** srv = reinterpret_cast<ID3D11ShaderResourceView**>(&inputs[0]);
	UINT numSrv = (UINT)inputs.size();

	Engine->GetContext()->PSSetShaderResources(4, numSrv, srv);

	for (size_t i = 0; i < inputs.size(); i++)
	{
		if (inputs[i] == nullptr)
		{
			postbuffer->data.InputSize[i].x = 0.0f;
			postbuffer->data.InputSize[i].y = 0.0f;
			continue;
		}

		ID3D11Texture2D* texture;
		D3D11_TEXTURE2D_DESC desc = {};
		D3D11_SHADER_RESOURCE_VIEW_DESC srvdesc = {};
		inputs[i]->GetDesc(&srvdesc);

		UINT mipLevel = srvdesc.Texture2D.MostDetailedMip;
		inputs[i]->GetResource((ID3D11Resource**)&texture);

		texture->GetDesc(&desc);

		postbuffer->data.InputSize[i].x = static_cast<float>(max<UINT>(desc.Width / (1 << mipLevel), 1));
		postbuffer->data.InputSize[i].y = static_cast<float>(max<UINT>(desc.Height / (1 << mipLevel), 1));

		texture->Release();
	}

	ID3D11Texture2D* ptexture;
	CD3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	D3D11_TEXTURE2D_DESC tex2DDesc = {};

	outputs[0]->GetResource((ID3D11Resource**)&ptexture);
	outputs[0]->GetDesc(&rtvDesc);

	UINT mipLevel = rtvDesc.Texture2D.MipSlice;

	ptexture->GetDesc(&tex2DDesc);

	postbuffer->data.OutputSize.x = static_cast<float>(max<UINT>(tex2DDesc.Width / (1 << mipLevel), 1));
	postbuffer->data.OutputSize.y = static_cast<float>(max<UINT>(tex2DDesc.Height / (1 << mipLevel), 1));

	postbuffer->SetPSBuffer(6);

	D3D11_VIEWPORT viewport[16];
	for (UINT i = 0; i < 16; ++i)
	{
		viewport[i].Width = static_cast<float>(max<UINT>(tex2DDesc.Width / (1 << mipLevel), 1));
		viewport[i].Height = static_cast<float>(max<UINT>(tex2DDesc.Height / (1 << mipLevel), 1));
		viewport[i].TopLeftX = 0;
		viewport[i].TopLeftY = 0;
		viewport[i].MinDepth = 0.0f;
		viewport[i].MaxDepth = 1.0f;
	}

	Engine->GetContext()->RSSetViewports(static_cast<UINT>(outputs.size()), viewport);
	
	Shader->SetPS();
	Engine->GetContext()->IASetInputLayout(nullptr);

	Engine->GetContext()->Draw(3, 0);

	ID3D11ShaderResourceView* nullSRVs[16] = { nullptr };
	Engine->GetContext()->PSSetShaderResources(3, static_cast<UINT>(inputs.size()), nullSRVs);

	ID3D11RenderTargetView* nullRTVs[16] = { nullptr };
	Engine->GetContext()->OMSetRenderTargets(static_cast<UINT>(outputs.size() + uaViews.size()), nullRTVs, nullptr);

	inputs.clear();
	outputs.clear();
	uaViews.clear();

	ptexture->Release();
}

