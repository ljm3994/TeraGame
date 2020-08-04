#pragma once
#include "FrameWork\Buffer\PostProcess\PostProcesPS.h"
struct RenderTarget
{
	ComPtr<ID3D11Texture2D> Texture;
	ComPtr<ID3D11RenderTargetView> rtv;
	ComPtr<ID3D11ShaderResourceView> srv;
	ComPtr<ID3D11UnorderedAccessView> uav;

	UINT width, height;

	DXGI_FORMAT format;
	UINT MSCount;
	UINT MSQuality;
	bool bInuse;
};
class PostProcess
{
protected:
	PostProcesPS* postbuffer;
	Shader* shader;
	UINT inputWidth;
	UINT inputHeight;
	vector<RenderTarget*> rendertargets;
	vector<ID3D11ShaderResourceView*>	inputs;
	vector<ID3D11RenderTargetView*>	outputs;
	vector<ID3D11UnorderedAccessView*> uaViews;
public:
	PostProcess();
	virtual ~PostProcess();

	virtual void Init();
	virtual void Render(ID3D11ShaderResourceView* input, ID3D11RenderTargetView* output);
	virtual void PostRender(UINT width, UINT height);
protected:
	RenderTarget* GetRenderTarget(UINT width, UINT height, DXGI_FORMAT format, UINT msCount = 1, UINT msQuality = 0,
		UINT mipLevels = 1, bool generateMipMaps = false, bool useAsUAV = false);

	void ClearRenderTargetCache();

	void Process(ID3D11ShaderResourceView* input, ID3D11RenderTargetView* output, Shader* Shader);
	virtual void Process(Shader* Shader);
};