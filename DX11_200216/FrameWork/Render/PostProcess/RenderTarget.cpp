#include "Framework.h"
#include "RenderTarget.h"

RenderTarget2::RenderTarget2(UINT Width, UINT Height)
{
    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width = Width;
    desc.Height = Height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;

    HRESULT hr = Engine->GetDevice()->CreateTexture2D(&desc, nullptr, rtvTexture.GetAddressOf());

    DEBUG(hr);

    D3D11_RENDER_TARGET_VIEW_DESC descRTV = {};
    descRTV.Format = desc.Format;
    descRTV.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

    hr = Engine->GetDevice()->CreateRenderTargetView(rtvTexture.Get(), &descRTV, rtv.GetAddressOf());

    DEBUG(hr);

    D3D11_SHADER_RESOURCE_VIEW_DESC descSRV = {};
    descSRV.Format = desc.Format;
    descSRV.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    descSRV.Texture2D.MipLevels = 1;

    hr = Engine->GetDevice()->CreateShaderResourceView(rtvTexture.Get(), &descSRV, srv.GetAddressOf());
    DEBUG(hr);
}

RenderTarget2::~RenderTarget2()
{
}

void RenderTarget2::Set(DepthStencil* depthstencil)
{
    Engine->GetContext()->OMSetRenderTargets(1, &rtv, depthstencil->GetDSV());
    Engine->GetContext()->ClearRenderTargetView(rtv.Get(), (float*)&D3DXCOLOR(1, 1, 1, 1));
    Engine->GetContext()->ClearDepthStencilView(depthstencil->GetDSV(), D3D11_CLEAR_DEPTH, 1.0f, 0);
}
