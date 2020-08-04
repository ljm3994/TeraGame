#include "Framework.h"
#include "Render2D.h"

Render2D::Render2D(UINT Width, UINT height)
{
    vp.Width = width;
    vp.Height = height;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;

    HRESULT hr;
    {//Create RTV Texture
        D3D11_TEXTURE2D_DESC desc = {};
        desc.Width = width;
        desc.Height = height;
        desc.MipLevels = 1;
        desc.ArraySize = 1;
        desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;

        hr = Engine->GetDevice()->CreateTexture2D(&desc, nullptr, rtvTexture.GetAddressOf());
        DEBUG(hr);
    }

    {//Create RTV
        D3D11_RENDER_TARGET_VIEW_DESC desc = {};
        desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
        desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

        hr = Engine->GetDevice()->CreateRenderTargetView(rtvTexture.Get(), &desc, rtv.GetAddressOf());
        DEBUG(hr);
    }

    {//Create SRV
        D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
        desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
        desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        desc.Texture2D.MipLevels = 1;

        hr = Engine->GetDevice()->CreateShaderResourceView(rtvTexture.Get(), &desc, srv.GetAddressOf());
        DEBUG(hr);
    }

    {//Create DSV Texture
        D3D11_TEXTURE2D_DESC desc = {};
        desc.Width = width;
        desc.Height = height;
        desc.MipLevels = 1;
        desc.ArraySize = 1;
        desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

        hr = Engine->GetDevice()->CreateTexture2D(&desc, nullptr, dsvTexture.GetAddressOf());
        DEBUG(hr);
    }

    {//Create DSV
        D3D11_DEPTH_STENCIL_VIEW_DESC desc = {};
        desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

        hr = Engine->GetDevice()->CreateDepthStencilView(dsvTexture.Get(), &desc, dsv.GetAddressOf());
        DEBUG(hr);
    }
}

Render2D::~Render2D()
{
}

void Render2D::Set(D3DXCOLOR color)
{
    Engine->GetContext()->OMSetRenderTargets(1, rtv.GetAddressOf(), dsv.Get());
    Engine->GetContext()->ClearRenderTargetView(rtv.Get(),color);
    Engine->GetContext()->ClearDepthStencilView(dsv.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

    Engine->GetContext()->RSSetViewports(1, &vp);
}
