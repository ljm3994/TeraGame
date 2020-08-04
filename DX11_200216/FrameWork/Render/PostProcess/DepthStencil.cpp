#include "Framework.h"
#include "DepthStencil.h"

DepthStencil::DepthStencil(UINT Width, UINT Height)
{
    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width = Width;
    desc.Height = Height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

    HRESULT hr = Engine->GetDevice()->CreateTexture2D(&desc, nullptr, dsvTexture.GetAddressOf());

    DEBUG(hr);

    CD3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
    descDSV.Format = desc.Format;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

    hr = Engine->GetDevice()->CreateDepthStencilView(dsvTexture.Get(), &descDSV, dsv.GetAddressOf());

    DEBUG(hr);
}

DepthStencil::~DepthStencil()
{
}
