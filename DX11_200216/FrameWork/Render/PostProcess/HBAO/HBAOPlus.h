#pragma once
#include <FrameWork\DXTypes\RenderTarget2D.h>

class GFSDK_SSAO_Context_D3D11;

struct HBAOParam
{
	bool EnableHBAOParam;
	bool EnableNormal;

	float MetersToViewSpace;

	float Radius;
	float Bias;
	float PowerExponent;
	float SmallScaleAO;
	float LargeScaleAO;
	bool EnableBlur;
	float BlurSharpness;

	HBAOParam()
		: EnableHBAOParam(true), EnableNormal(true), MetersToViewSpace(10.0f), Radius(1.0f), Bias(5.0f), PowerExponent(0.3f),
		SmallScaleAO(1.0f), LargeScaleAO(1.0f), EnableBlur(true), BlurSharpness(8.0f)
	{

	}
};

class HBAOPlus
{
private:
	GFSDK_SSAO_Context_D3D11* AOContext;
	RenderTarget2D HBAOrt;

	HBAOParam* hbaoparams;
public:
	HBAOPlus();

	bool Init(ID3D11Device* pDevice, UINT Width, UINT Height, HBAOParam* const HbaoParam);
	void CleanUp();

	void Render(ID3D11DeviceContext* Context, const D3DXMATRIX& projMat, D3DXMATRIX viewMat, ID3D11ShaderResourceView* DepthSRV, ID3D11ShaderResourceView* NormalsSRV);

	ID3D11ShaderResourceView* GetHBAOSRV() const;
};