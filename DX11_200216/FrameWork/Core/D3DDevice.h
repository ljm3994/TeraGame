#pragma once
#include "Framework/Core/System.h"

class D3DDevice : public System
{
protected:
	ComPtr<ID3D11Device> DXDevice;
	ComPtr<ID3D11DeviceContext> Context;
	ComPtr<IDXGISwapChain> SwapChain;
	ComPtr<ID3D11RasterizerState> Rasterrizer;
	ComPtr<ID3D11RenderTargetView> RenderTargetView;
	ComPtr<ID3D11DepthStencilView> DepthView;
	ComPtr<ID3D11DepthStencilState> DepthState;
	ComPtr<ID3D11Texture2D> Depth;

	D3D11_VIEWPORT vp;
	UINT VideoCardMemory;
	string VideoCardDesc;
	bool IsVsyncy;
	bool SetSwapChain();
	bool SetDevice();
	void GetGrapicCardRefreshRate(OUT UINT& numerator, OUT UINT& denominator);
	bool SetRasterRizer();
	virtual void SizeChanged(WPARAM wparam) override;
	virtual void OnResize() override;

	void BeginRender(D3DXCOLOR clear = D3DXCOLOR(1, 1, 1, 1));
	void EndRender();
public:
	D3DDevice(HINSTANCE hinstance, wstring AppName = L"DX11", bool Enable4xMsaa = true, bool isVsync = false);
	virtual ~D3DDevice();
	bool InitDevice();

	virtual int Run() override;
};