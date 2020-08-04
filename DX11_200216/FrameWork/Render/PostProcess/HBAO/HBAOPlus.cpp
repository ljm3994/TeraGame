#include "Framework.h"
#include "HBAOPlus.h"

HBAOPlus::HBAOPlus()
	: AOContext(nullptr), hbaoparams(nullptr)
{
}

bool HBAOPlus::Init(ID3D11Device* pDevice, UINT Width, UINT Height, HBAOParam* const HbaoParam)
{
	if (!HbaoParam)
		return false;

	hbaoparams = HbaoParam;

	GFSDK_SSAO_CustomHeap CustomHeap;
	CustomHeap.new_ = ::operator new;
	CustomHeap.delete_ = ::operator delete;

	GFSDK_SSAO_Status status;
	status = GFSDK_SSAO_CreateContext_D3D11(pDevice, &AOContext, &CustomHeap);

	if (status == GFSDK_SSAO_OK)
	{
		HBAOrt.Init(Width, Height, DXGI_FORMAT_R32_FLOAT);

		return true;
	}
	else
	{
		return false;
	}
}

void HBAOPlus::CleanUp()
{
	AOContext->Release();

	HBAOrt.Release();
}

void HBAOPlus::Render(ID3D11DeviceContext* Context, const D3DXMATRIX& projMat, D3DXMATRIX viewMat, ID3D11ShaderResourceView* DepthSRV, ID3D11ShaderResourceView* NormalsSRV)
{
	if (hbaoparams->EnableHBAOParam)
	{
		GFSDK_SSAO_InputData_D3D11 Input;

		Input.DepthData.DepthTextureType = GFSDK_SSAO_HARDWARE_DEPTHS;
		Input.DepthData.pFullResDepthTextureSRV = DepthSRV;
		Input.DepthData.ProjectionMatrix.Data = GFSDK_SSAO_Float4x4((const float*)&projMat);
		Input.DepthData.ProjectionMatrix.Layout = GFSDK_SSAO_ROW_MAJOR_ORDER;
		Input.DepthData.MetersToViewSpaceUnits = hbaoparams->MetersToViewSpace;

		Input.NormalData.Enable = hbaoparams->EnableNormal;
		Input.NormalData.pFullResNormalTextureSRV = NormalsSRV;
		
		Input.NormalData.WorldToViewMatrix.Data = GFSDK_SSAO_Float4x4((GFSDK_SSAO_FLOAT*)&viewMat);
		Input.NormalData.WorldToViewMatrix.Layout = GFSDK_SSAO_ROW_MAJOR_ORDER;
		Input.NormalData.DecodeScale = 2.f;
		Input.NormalData.DecodeBias = -1.f;

		GFSDK_SSAO_Output_D3D11 Output;
		Output.pRenderTargetView = HBAOrt.rtv;
		Output.Blend.Mode = GFSDK_SSAO_OVERWRITE_RGB;

		GFSDK_SSAO_Parameters AOParams;
		AOParams.Radius = hbaoparams->Radius;
		AOParams.Bias = hbaoparams->Bias;
		AOParams.SmallScaleAO = hbaoparams->SmallScaleAO;
		AOParams.LargeScaleAO = hbaoparams->LargeScaleAO;
		AOParams.PowerExponent = hbaoparams->PowerExponent;
		AOParams.Blur.Enable = hbaoparams->EnableBlur;
		AOParams.Blur.Sharpness = hbaoparams->BlurSharpness;
		AOParams.Blur.Radius = GFSDK_SSAO_BLUR_RADIUS_4;

		const GFSDK_SSAO_RenderMask RenderMask = GFSDK_SSAO_RENDER_AO;

		GFSDK_SSAO_Status Status;
		Status = AOContext->RenderAO(Context, Input, AOParams, Output, RenderMask);
		assert(Status == GFSDK_SSAO_OK);
	}
	else
	{
		const D3DXCOLOR ClearColor = { 1.0f, 1.0f, 1.0f, 1.0f };

		Context->ClearRenderTargetView(HBAOrt.rtv, ClearColor);
	}
}

ID3D11ShaderResourceView* HBAOPlus::GetHBAOSRV() const
{
	return HBAOrt.srv;
}
