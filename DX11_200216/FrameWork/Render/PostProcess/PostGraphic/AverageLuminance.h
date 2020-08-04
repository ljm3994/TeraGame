#pragma once
#include <FrameWork\DXTypes\RenderTarget2D.h>

class FullScreenPass;
template <typename type>
class StructureBuffer;
template <typename type>
class ConstantBuffer;

class AverageLuminance
{
private:
	RenderTarget2D texLog;
	RenderTarget2D texAdapted[2];
	UINT CurrLumTarget;

	Shader* PSShaderToneMapping;
	Shader* PSShaderToneMapAdapted;

	Shader* ComputeShaderPhase1;
	Shader* ComputeShaderPhase2;

	StructureBuffer<UINT>* csBuffer;
	RenderTarget2D renderTargetQuterRes;

	struct Data
	{
		D3DXVECTOR2 QuarterSize;
		D3DXVECTOR2 params;

		D3DXVECTOR4 viewPortFull;
		D3DXVECTOR4 params2;
	};

	ConstantBuffer<Data>* buffer;

	FullScreenPass* fullscreenpass;

public:
	AverageLuminance();
	~AverageLuminance();

	void Init(FullScreenPass* fullscreenpass, UINT fullscreenwidth, UINT fullscreenheight, const UINT luminanceMips = 11);
	void Release();

	void RenderAvgLuminanceAndEyeAdaptation(ID3D11ShaderResourceView* InputColor, ID3D11ShaderResourceView* DepthSRV);
	void CalcAvgLuminance(ID3D11ShaderResourceView* InputColor, ID3D11ShaderResourceView* DepthSRV);

	const RenderTarget2D& GetAdapted() const;

private:
	void Render(ID3D11ShaderResourceView* InputColor);
	void RenderEyeAdaptation();
};