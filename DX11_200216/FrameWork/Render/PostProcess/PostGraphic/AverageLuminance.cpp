#include "Framework.h"
#include "AverageLuminance.h"
#include "FrameWork\Render\PostProcess\FullScreenPass.h"
#include "FrameWork\Buffer\Base\StructuredBuffer.h"

AverageLuminance::AverageLuminance()
	: CurrLumTarget(0)
{
}

AverageLuminance::~AverageLuminance()
{
}

void AverageLuminance::Init(FullScreenPass* fullscreenpass, UINT fullscreenwidth, UINT fullscreenheight, const UINT luminanceMips)
{
	this->fullscreenpass = fullscreenpass;

	PSShaderToneMapAdapted = Engine->AddShader("AdaptedLuminance", L"PSToneMappingCalcAdaptedLuminance", ShaderCreateFlag::SHADER_FLAG_PS);
	PSShaderToneMapping = Engine->AddShader("Luminance", L"PSToneMappingLogLuminance", ShaderCreateFlag::SHADER_FLAG_PS);

	texLog.Init(1 << (luminanceMips - 1), 1 << (luminanceMips - 1), DXGI_FORMAT_R16_FLOAT, false, true, luminanceMips);

	for (UINT i = 0; i < 2; ++i)
	{
		texAdapted[i].Init(1, 1, DXGI_FORMAT_R16_FLOAT, true);
	}

	csBuffer = new StructureBuffer<UINT>(256, D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE);

	renderTargetQuterRes.Init(fullscreenwidth / 4, fullscreenheight / 4, DXGI_FORMAT_R11G11B10_FLOAT, false);

	ComputeShaderPhase1 = Engine->AddShader("Phase1", L"TW3_Phase1", ShaderCreateFlag::SHADER_FLAG_CS);
	ComputeShaderPhase2 = Engine->AddShader("Phase2", L"TW3_Phase2", ShaderCreateFlag::SHADER_FLAG_CS);

	buffer = new ConstantBuffer<Data>();

	buffer->GetData().QuarterSize = D3DXVECTOR2((float)renderTargetQuterRes.Width, (float)renderTargetQuterRes.Height);
	buffer->GetData().params = D3DXVECTOR2(0.266f, 0.766f);
	buffer->GetData().params2 = D3DXVECTOR4(0.22f, 0.53f, 4.0f, 0.0f);
	buffer->GetData().viewPortFull = D3DXVECTOR4((float)fullscreenwidth, (float)fullscreenheight, 0.0f, 0.0f);
}

void AverageLuminance::Release()
{
	texLog.Release();
	
	for (UINT i = 0; i < 2; i++)
	{
		texAdapted[i].Release();
	}

	renderTargetQuterRes.Release();
	delete csBuffer;
	delete buffer;
}

void AverageLuminance::RenderAvgLuminanceAndEyeAdaptation(ID3D11ShaderResourceView* InputColor, ID3D11ShaderResourceView* DepthSRV)
{
	Render(InputColor);

	RenderEyeAdaptation();

	CurrLumTarget = !CurrLumTarget;
}

void AverageLuminance::CalcAvgLuminance(ID3D11ShaderResourceView* InputColor, ID3D11ShaderResourceView* DepthSRV)
{
	fullscreenpass->RenderScaling(renderTargetQuterRes.Width, renderTargetQuterRes.Height, InputColor, renderTargetQuterRes.rtv);

	const UINT values[4] = { 0, 0, 0, 0 };

	Engine->GetContext()->ClearUnorderedAccessViewUint(csBuffer->GetUAV(), values);

	ID3D11ShaderResourceView* pInputsSRV[2] = { InputColor, DepthSRV };
	Engine->GetContext()->CSSetShaderResources(3, 2, pInputsSRV);

	ID3D11UnorderedAccessView* pBufferUAV[2] = { csBuffer->GetUAV(), texAdapted[CurrLumTarget].uav };
	Engine->GetContext()->CSSetUnorderedAccessViews(0, 1, pBufferUAV, nullptr);

	buffer->SetCSBuffer(5);

	ComputeShaderPhase1->SetCS();

	Engine->GetContext()->Dispatch((UINT)renderTargetQuterRes.Height, 1, 1);

	ComputeShaderPhase2->SetCS();

	Engine->GetContext()->CSSetUnorderedAccessViews(0, 2, pBufferUAV, nullptr);

	Engine->GetContext()->Dispatch(1, 1, 1);

}

const RenderTarget2D& AverageLuminance::GetAdapted() const
{
	return texAdapted[!CurrLumTarget];
}

void AverageLuminance::Render(ID3D11ShaderResourceView* InputColor)
{
	ID3D11RenderTargetView* RTViews[1] = { texLog.rtv };
	Engine->GetContext()->OMSetRenderTargets(1, RTViews, nullptr);

	ID3D11ShaderResourceView* pInputTextures[1] = { InputColor };
	Engine->GetContext()->PSSetShaderResources(0, 1, pInputTextures);

	PSShaderToneMapping->SetPS();
	
	fullscreenpass->Render(texLog.Width, texLog.Height);

	Engine->GetContext()->GenerateMips(texLog.srv);
}

void AverageLuminance::RenderEyeAdaptation()
{
	ID3D11RenderTargetView* pAdaptedLuminanceRTV = { texAdapted[CurrLumTarget].rtv };

	float ClearColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	Engine->GetContext()->ClearRenderTargetView(pAdaptedLuminanceRTV, ClearColor);

	ID3D11ShaderResourceView* pLastLuminanceSRV = { texAdapted[!CurrLumTarget].srv };
	ID3D11ShaderResourceView* pInputsSRV[2] = { texLog, pLastLuminanceSRV };

	Engine->GetContext()->OMSetRenderTargets(1, &pAdaptedLuminanceRTV, nullptr);
	Engine->GetContext()->PSSetShaderResources(0, 2, pInputsSRV);
	
	PSShaderToneMapAdapted->SetPS();

	fullscreenpass->Render(1, 1);
}
