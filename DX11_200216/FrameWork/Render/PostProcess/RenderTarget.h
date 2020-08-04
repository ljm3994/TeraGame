#pragma once
#include <FrameWork\Render\PostProcess\DepthStencil.h>

class RenderTarget2
{
private:
	ComPtr<ID3D11ShaderResourceView> srv;
	ComPtr<ID3D11Texture2D> rtvTexture;
	ComPtr<ID3D11RenderTargetView> rtv;
public:
	RenderTarget2(UINT Width, UINT Height);
	~RenderTarget2();

	void Set(DepthStencil* depthstencil);

	ID3D11ShaderResourceView* GetSRV() { return srv.Get(); }
	ID3D11RenderTargetView* GetRTV() { return rtv.Get(); }
};