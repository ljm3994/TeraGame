#include "Framework.h"
#include "PostProcessRender.h"
#include "Object\SkyBox.h"

PostProcessRender::PostProcessRender()
{
	fxaa = new FXAA();
}

PostProcessRender::~PostProcessRender()
{
	delete fxaa;
	delete gbuffer;
	delete depthState[0];
	delete depthState[1];
	delete depthState[2];
	delete resterCullBackFace;
	delete resterCullMSAA;
	delete resterCullNo;
	delete OrginalRs;
	delete blend[0];
	delete blend[1];
	delete bloomBlendState;
	averageluminance.Release();
	lensflares.Release();
}

void PostProcessRender::Init(ID3D11RenderTargetView* Backbuffer)
{
	UINT Width = Engine->GetClientWidth();
	UINT Height = Engine->GetClientHeight();

	basicpostprocess = make_unique<DirectX::BasicPostProcess>(Engine->GetDevice());
	Tonepostprocess = make_unique<DirectX::ToneMapPostProcess>(Engine->GetDevice());
	Dualpostprocess = make_unique<DirectX::DualPostProcess>(Engine->GetDevice());
	OrginalRs = new RasterizeState();
	resterCullBackFace = new RasterizeState();
	resterCullMSAA = new RasterizeState();
	resterCullNo = new RasterizeState();
	resterCullBackFace->FrontCounterClockwise(false);
	auto& RSDesc = resterCullBackFace->GetDesc();
	RSDesc.DepthBias = false;
	RSDesc.DepthBiasClamp = 0;
	RSDesc.SlopeScaledDepthBias = 0;
	RSDesc.DepthClipEnable = false;
	RSDesc.ScissorEnable = false;
	RSDesc.MultisampleEnable = false;
	RSDesc.AntialiasedLineEnable = false;
	resterCullBackFace->Changed();

	auto& RSDesc2 = resterCullMSAA->GetDesc();
	RSDesc2.DepthBias = false;
	RSDesc2.DepthBiasClamp = 0;
	RSDesc2.SlopeScaledDepthBias = 0;
	RSDesc2.DepthClipEnable = false;
	RSDesc2.ScissorEnable = false;
	RSDesc2.MultisampleEnable = true;
	RSDesc2.AntialiasedLineEnable = false;
	RSDesc2.FrontCounterClockwise = false;
	resterCullMSAA->Changed();

	auto& RSDesc3 = resterCullNo->GetDesc();
	RSDesc3.DepthBias = false;
	RSDesc3.DepthBiasClamp = 0;
	RSDesc3.SlopeScaledDepthBias = 0;
	RSDesc3.DepthClipEnable = false;
	RSDesc3.ScissorEnable = false;
	RSDesc3.MultisampleEnable = true;
	RSDesc3.AntialiasedLineEnable = false;
	RSDesc3.FrontCounterClockwise = false;
	RSDesc3.CullMode = D3D11_CULL_NONE;
	resterCullNo->Changed();

	toneMappingparams.CurveABCD.x = 0.98506f;
	toneMappingparams.CurveABCD.y = 1.03957f;
	toneMappingparams.CurveABCD.z = 1.03862f;;
	toneMappingparams.CurveEF.x = 0.025f;
	toneMappingparams.CurveEF.y = 1.04592f;;

	toneMappingparams.MinMaxAllowedSceneLuminance.x = 0.0f;
	toneMappingparams.powParam = 0.45f;
	toneMappingparams.WhitPoint = 1.03f;
	toneMappingparams.NominatorMult = 0.8912f;

	this->Backbuffer = Backbuffer;

	DefferedPSShader = Engine->AddShader("DeferredShading", L"DeferredShading", ShaderCreateFlag::SHADER_FLAG_PS);
	DefferedCSShader = Engine->AddShader("DeferredShadingCS", L"DeferredShadingCS", ShaderCreateFlag::SHADER_FLAG_CS);
	BloomPSThreshold = Engine->AddShader("Bloom", L"Bloom", ShaderCreateFlag::SHADER_FLAG_PS, "", "BloomThreshold");
	BloomPSShaderBlurH = Engine->AddShader("BloomH", L"Bloom", ShaderCreateFlag::SHADER_FLAG_PS, "", "BlurH");
	BloomPSShaderBlurV = Engine->AddShader("BloomV", L"Bloom", ShaderCreateFlag::SHADER_FLAG_PS, "", "BlurV");
	MotionBlurPSShader = Engine->AddShader("MotionBlur", L"MotionBlur", ShaderCreateFlag::SHADER_FLAG_PS);
	PSShaderFinalPostProcess = Engine->AddShader("PostProcessFinalStage", L"PostProcessFinalStage", ShaderCreateFlag::SHADER_FLAG_PS);
	PSShaderToneMapping = Engine->AddShader("ToneMapping", L"ToneMapping", ShaderCreateFlag::SHADER_FLAG_PS);
	PSShaderFinalPostProcessShowVignette = Engine->AddShader("PostProcessFinalStage", L"PostProcessFinalStage", ShaderCreateFlag::SHADER_FLAG_PS);
	SkyboxVelocity = Engine->AddShader("SkyboxVelocity", L"SkyBoxVelocity");

	depthState[0] = new DepthStencilState();
	depthState[0]->DepthEnable(true);
	depthState[0]->DepthFunc(D3D11_COMPARISON_GREATER_EQUAL);
	depthState[0]->StencilEnable(false);
	depthState[2] = new DepthStencilState();
	depthState[2]->DepthEnable(false);

	depthState[1] = new DepthStencilState();

	blend[0] = new BlendState();
	blend[1] = new BlendState();
	blend[0]->Alpha(true);

	bloomBlendState = new BlendState();
	bloomBlendState->RenderTargetSet(true, D3D11_BLEND_ONE, D3D11_BLEND_ONE, D3D11_BLEND_OP_ADD, D3D11_BLEND_OP_ADD,
		D3D11_BLEND_ONE, D3D11_BLEND_ONE, D3D11_COLOR_WRITE_ENABLE_ALL);

	lensflares.Init(&fullscreenpass, Width, Height);
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	memset(&samplerDesc.BorderColor, 0, sizeof(samplerDesc.BorderColor));
	samplerDesc.MinLOD = -FLT_MAX;
	samplerDesc.MaxLOD = FLT_MAX;

	Engine->GetDevice()->CreateSamplerState(&samplerDesc, SamplerLinearClamp.GetAddressOf());

	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.MaxAnisotropy = 8;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0;
	samplerDesc.MipLODBias = -1.0f;

	Engine->GetDevice()->CreateSamplerState(&samplerDesc, SamplerAnisoClamp.GetAddressOf());

	{//Create DSV Texture
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

		Engine->GetDevice()->CreateTexture2D(&desc, nullptr, dsvTexture.GetAddressOf());
	}

	{//Create DSV
		D3D11_DEPTH_STENCIL_VIEW_DESC desc = {};
		desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

		Engine->GetDevice()->CreateDepthStencilView(dsvTexture.Get(), &desc, dsv.GetAddressOf());
	}

	gbuffer = new GBuffer();
	RenderTargetColor.Init(Width, Height, DXGI_FORMAT_R16G16B16A16_FLOAT, true);
	RenderTargetFXAAProxy.Init(Width, Height, DXGI_FORMAT_R16G16B16A16_FLOAT);
	motionBlurTex.Init(Width, Height, DXGI_FORMAT_R16G16B16A16_FLOAT);
	BloomTexHalfRes.Init(Width / 2, Height / 2, DXGI_FORMAT_R16G16B16A16_FLOAT);

	for (UINT i = 0; i < 2; i++)
	{
		bloomTex[i].Init(Width / 4, Height / 4, DXGI_FORMAT_R11G11B10_FLOAT);
	}

	textureLUT = Engine->AddTexture("LUT", L"Texture/Effect/LUT.dds", false, true);
	texLensDirt = Engine->AddTexture("LensDirt", L"Texture/Effect/lensdirt.png", false);
	texLensStar = Engine->AddTexture("LensStart", L"Texture/Effect/lensstar.png", false);
	textureNoise = Engine->AddTexture("Noise", L"Texture/Effect/noise.dds", false, true);
	textureVignetteMask = Engine->AddTexture("Vignettemask", L"Texture/Effect/vignette.dds", false, true);
	texturenormalFit = Engine->AddTexture("TexNormalFit", L"Texture/Effect/w3_normals_encode.dds", false, true);

	fxaa->Init();
	hbaoplus.Init(Engine->GetDevice(), Engine->GetClientWidth(), Engine->GetClientHeight(), &hbaoParam);
	fullscreenpass.Create();
	averageluminance.Init(&fullscreenpass, Engine->GetClientWidth(), Engine->GetClientHeight(), 10);
	lensflares.Init(&fullscreenpass, Engine->GetClientWidth(), Engine->GetClientHeight());
	sharpen.Init(&fullscreenpass);
	gbuffer->Init(Engine->GetClientWidth(), Engine->GetClientHeight());
}

void PostProcessRender::Update(float DeltaTime)
{
	
}

void PostProcessRender::Set()
{
	PrePareRender();

	static const float ClearColor[4] = { 0.f, 0.f, 0.f, 0.f };

	Engine->GetContext()->ClearRenderTargetView(Backbuffer, ClearColor);
	Engine->GetContext()->ClearRenderTargetView(RenderTargetColor.rtv, ClearColor);
	Engine->GetContext()->ClearRenderTargetView(RenderTargetFXAAProxy.rtv, ClearColor);

	const float DepthClearValue = (isReversedDepth) ? 0.0f : 1.0f;
	gbuffer->ClearGBuffer(DepthClearValue, isClearGBuffer);

	vp.Width = static_cast<float>(RenderTargetColor.Width);
	vp.Height = static_cast<float>(RenderTargetColor.Height);
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	Engine->GetContext()->RSSetViewports(1, &vp);

	gbuffer->Set();

	//if (isReversedDepth)
	//{
	//	depthState[0]->Set(0);
	//}
	//else
	//{
	//	depthState[1]->Set(0);
	//}

	texturenormalFit->PSSet(13);

	Engine->GetEnviroment()->Render();
	Engine->GetCurrentScene()->SceneRender();
}

void PostProcessRender::Render()
{
	blend[1]->Set(D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));
	
	resterCullNo->Set();

	viewdata.SetVSBuffer(3);
	viewdata.SetPSBuffer(3);
	framebuffer.SetVSBuffer(7);
	framebuffer.SetPSBuffer(7);
	lightbuffer.SetPSBuffer(4);
	lightbuffer.SetVSBuffer(4);

	ID3D11ShaderResourceView* pDepthSRV = gbuffer->depthstencilbuffer;
	ID3D11ShaderResourceView* pNormalsSRV = gbuffer->RenderTarget1;

	hbaoplus.Render(Engine->GetContext(),
		Engine->GetProjectionMatrix(),
		Engine->GetCurrentCamera()->GetView(),
		pDepthSRV, pNormalsSRV);

	ID3D11ShaderResourceView* GBufferSRV[] =
	{
		gbuffer->depthstencilbuffer.SRV,
		gbuffer->RenderTarget0.srv,
		gbuffer->RenderTarget1.srv,
		gbuffer->RenderTarget2.srv,
		hbaoplus.GetHBAOSRV()
	};
	
	if (isDefrredShadingUseComputeShader == false)
	{
		ID3D11RenderTargetView* RTViews[1] = { RenderTargetColor.rtv };
		Engine->GetContext()->OMSetRenderTargets(1, RTViews, nullptr);
	
		Engine->GetContext()->PSSetShaderResources(3, 5, GBufferSRV);
	
		DefferedPSShader->SetPS();
	
		fullscreenpass.RenderTri();
	
		for (UINT i = 0; i < ARRAYSIZE(GBufferSRV); ++i)
			GBufferSRV[i] = nullptr;
	
		Engine->GetContext()->PSSetShaderResources(3, 5, GBufferSRV);
	}
	else
	{
		ID3D11RenderTargetView* pNullRTV[3] = { nullptr, nullptr, nullptr };
		Engine->GetContext()->OMSetRenderTargets(3, pNullRTV, nullptr);
	
		ID3D11UnorderedAccessView* UAViews[1] = { RenderTargetColor.uav };
		UINT nInitialsCounts[1] = { 0 };
		Engine->GetContext()->CSSetUnorderedAccessViews(0, 1, UAViews, nInitialsCounts);
	
		Engine->GetContext()->CSSetShaderResources(3, 5, GBufferSRV);
		DefferedCSShader->SetCS();
	
		UINT tilesX = (UINT)ceil(gbuffer->RenderTarget0.Width / 16.0f);
		UINT tilesY = (UINT)ceil(gbuffer->RenderTarget0.Height / 16.0f);
		Engine->GetContext()->Dispatch(tilesX, tilesY, 1);
	
		for (UINT i = 0; i < ARRAYSIZE(GBufferSRV); ++i)
			GBufferSRV[i] = nullptr;
	
		Engine->GetContext()->CSSetShaderResources(3, 5, GBufferSRV);
		UAViews[0] = nullptr;
		Engine->GetContext()->CSSetUnorderedAccessViews(0, 1, UAViews, nInitialsCounts);
	}

	//ID3D11RenderTargetView* RTViews[1] = { motionBlurTex.rtv };
	//Engine->GetContext()->OMSetRenderTargets(1, RTViews, nullptr);
	//
	//basicpostprocess->SetEffect(DirectX::BasicPostProcess::BloomExtract);
	//basicpostprocess->SetBloomExtractParameter(1.0f);
	//basicpostprocess->SetSourceTexture(RenderTargetColor.srv);
	//basicpostprocess->Process(Engine->GetContext());

	ID3D11RenderTargetView* RTViews2[1] = { RenderTargetFXAAProxy.rtv };
	Engine->GetContext()->OMSetRenderTargets(1, RTViews2, nullptr);
	
	basicpostprocess->SetEffect(DirectX::BasicPostProcess::BloomBlur);
	basicpostprocess->SetSourceTexture(RenderTargetColor.srv);
	
	basicpostprocess->Process(Engine->GetContext());
	//
	//ID3D11RenderTargetView* RTViews3[1] = { RenderTargetFXAAProxy.rtv };
	//
	//Engine->GetContext()->OMSetRenderTargets(1, RTViews3, nullptr);
	//
	//Dualpostprocess->SetEffect(DirectX::DualPostProcess::Effect::BloomCombine);
	//Dualpostprocess->SetSourceTexture(bloomTex[0].srv);
	//Dualpostprocess->SetSourceTexture2(motionBlurTex.srv);
	////Dualpostprocess->SetBloomCombineParameters(0.5f, 0.3f, 0.7f, 2.5f);
	//Dualpostprocess->Process(Engine->GetContext());

	ID3D11RenderTargetView* RTViews4[1] = { Backbuffer };
	Engine->GetContext()->OMSetRenderTargets(1, RTViews4, nullptr);
	
	Tonepostprocess->SetOperator(DirectX::ToneMapPostProcess::Operator::Reinhard);
	Tonepostprocess->SetHDRSourceTexture(RenderTargetFXAAProxy.srv);
	
	Tonepostprocess->Process(Engine->GetContext());
	/////////////////////////Sky//////////////////////////////////
	
	//sky->Render();
	//ID3D11RenderTargetView* renderTargets[1];
	//renderTargets[0] = RenderTargetColor.rtv;
	//Engine->GetContext()->OMSetRenderTargets(1, renderTargets, gbuffer->depthstencilbuffer.DSVReadOnly);
	//
	//D3D11_VIEWPORT skyViewPort = vp;
	//skyViewPort.MinDepth = (isReversedDepth) ? 0.f : 1.f;
	//skyViewPort.MaxDepth = skyViewPort.MinDepth;
	//Engine->GetContext()->RSSetViewports(1, &skyViewPort);
	//
	//ID3D11ShaderResourceView* SkyDepthSRV = gbuffer->depthstencilbuffer.SRV ;
	//Engine->GetContext()->PSSetShaderResources(10, 1, &SkyDepthSRV);
	//resterCullBackFace->Set();
	//
	//sky->Render(true, true, true);
	//
	//ID3D11ShaderResourceView* SkyDepthSRVNull = nullptr;
	//Engine->GetContext()->PSSetShaderResources(10, 1, &SkyDepthSRVNull);
	//
	//ID3D11RenderTargetView* SkyrenderTargets = gbuffer->RenderTarget3.rtv;
	//
	//Engine->GetContext()->OMSetRenderTargets(1, &SkyrenderTargets, nullptr);
	//
	//bloomBlendState->Set(D3DXVECTOR4(1.f, 1.f, 1.f, 1.f));
	//
	//depthState[2]->Set(0);
	//
	//SkyboxVelocity->SetRender();
	//
	//sky->DrawSkySphere();
	//
	//Engine->GetContext()->OMSetBlendState(nullptr, D3DXVECTOR4(1.f, 1.f, 1.f, 1.f), 0xFFFFFFFF);
	//resterCullBackFace->Set();

	/////////////////////////////////////////averageluminance////////////////////////////////////////////////////////
	//averageluminance.RenderAvgLuminanceAndEyeAdaptation(RenderTargetColor.srv, gbuffer->depthstencilbuffer.SRV);

	/////////////////////////MotionBlur////////////////////////////////////////
	//D3D11_VIEWPORT mbViewport;
	//mbViewport.Width = (float)RenderTargetColor.Width;
	//mbViewport.Height = (float)RenderTargetColor.Height;
	//mbViewport.TopLeftX = 0.0f;
	//mbViewport.TopLeftY = 0.0f;
	//mbViewport.MinDepth = 0.0f;
	//mbViewport.MaxDepth = 1.0f;
	//Engine->GetContext()->RSSetViewports(1, &mbViewport);
	//
	//ID3D11RenderTargetView* pOutputRTV[1] = { motionBlurTex.rtv };
	//Engine->GetContext()->OMSetRenderTargets(1, pOutputRTV, nullptr);
	//
	//ID3D11ShaderResourceView* pInputsSRV[2] = { RenderTargetColor.srv, gbuffer->RenderTarget3.srv };
	//Engine->GetContext()->PSSetShaderResources(3, 2, pInputsSRV);
	//
	//MotionBlurPSShader->SetPS();
	//
	//fullscreenpass.RenderTri();
	//
	////////////////////////////bloom/////////////////////////////////////////////
	//
	//fullscreenpass.RenderScaling(BloomTexHalfRes.Width, BloomTexHalfRes.Height, motionBlurTex.srv, BloomTexHalfRes.rtv);
	//
	//UINT nBloomWidth = bloomTex[0].Width;
	//UINT nBloomHeigth = bloomTex[0].Height;
	//UINT nBloomTexIndex = 0;
	//
	//fullscreenpass.Render(nBloomWidth, nBloomHeigth, BloomTexHalfRes.srv, bloomTex[nBloomTexIndex].rtv, BloomPSThreshold);
	//
	//for (UINT i = 0; i < 3; i++)
	//{
	//	ID3D11ShaderResourceView* pNullSRV[1] = { nullptr };
	//	Engine->GetContext()->PSSetShaderResources(3, 1, pNullSRV);
	//
	//	fullscreenpass.Render(nBloomWidth, nBloomHeigth, bloomTex[nBloomTexIndex].srv, bloomTex[!nBloomTexIndex].rtv, BloomPSShaderBlurH);
	//
	//	Engine->GetContext()->PSSetShaderResources(3, 1, pNullSRV);
	//	fullscreenpass.Render(nBloomWidth, nBloomHeigth, bloomTex[!nBloomTexIndex].srv, bloomTex[nBloomTexIndex].rtv, BloomPSShaderBlurV);
	//}
	
	////////////////////////////////LensFlares/////////////////////////////////////////////
	//lensflares.Render(RenderTargetColor.srv);
	//
	////////////////////////////////ToneMapping///////////////////////////////////////////
	//
	//resterCullBackFace->Set();
	//ID3D11RenderTargetView* RTViews[1] = { RenderTargetFXAAProxy.rtv };
	//Engine->GetContext()->OMSetRenderTargets(1, RTViews, nullptr);
	//
	//ID3D11ShaderResourceView* SRVs[] =
	//{
	//	motionBlurTex.srv,
	//	averageluminance.GetAdapted().srv,
	//	textureLUT->GetSrv(),
	//	gbuffer->depthstencilbuffer.SRV,
	//	bloomTex[0].srv,
	//	lensflares.renderTargetHalfRes2.srv,
	//	texLensDirt->GetSrv(),
	//	texLensStar->GetSrv()
	//};
	//
	//Engine->GetContext()->PSSetShaderResources(3, 8, SRVs);
	//
	//toneMapping.GetData() = toneMappingparams;
	//toneMapping.SetPSBuffer(5);
	//
	//PSShaderToneMapping->SetPS();
	//
	//fullscreenpass.RenderTri();
	//
	//SRVs[0] = nullptr;
	//Engine->GetContext()->PSSetShaderResources(3, 1, SRVs);
	//
	//////////////////////////////////////////postprocessupdate///////////////////////////
	//postbuffer.SetPSBuffer(1);
	//postbuffer.SetVSBuffer(1);
	//
	/////////////////////////////////////////Sharpen//////////////////////////////////////
	//ID3D11RenderTargetView* pRTVOutput = { motionBlurTex.rtv };
	//ID3D11ShaderResourceView* pSRVInputs[] =
	//{
	//	RenderTargetFXAAProxy.srv,
	//	gbuffer->depthstencilbuffer.SRV
	//};
	//
	//sharpen.Render(pSRVInputs, _countof(pSRVInputs), pRTVOutput);
	//
	/////////////////////////////////////////////2´Ü°è/////////////////////////////////////////////////////////
	//textureNoise->PSSet(4);
	//textureVignetteMask->PSSet(5);
	//
	//if (isShowVignette)
	//{
	//	fullscreenpass.Render(RenderTargetColor.Width, RenderTargetColor.Height, motionBlurTex.srv, RenderTargetColor.rtv, PSShaderFinalPostProcessShowVignette);
	//}
	//else
	//{
	//	fullscreenpass.Render(RenderTargetColor.Width, RenderTargetColor.Height, motionBlurTex.srv, RenderTargetColor.rtv, PSShaderFinalPostProcess);
	//}
	//
	/////////////////////////¾ÈÆ¼¿¤¸®¾î½Ì////////////////////////////////////////////
	//if (isDebugView)
	//{
	//	fxaa->Render(RenderTargetColor.srv, Backbuffer);
	//}
	//else
	//{
	//	fxaa->Render(RenderTargetColor.srv, Backbuffer);
	//}
	
	OrginalRs->Set();
	blend[1]->Set();
	depthState[1]->Set();
	ID3D11ShaderResourceView* pSRV[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	Engine->GetContext()->PSSetShaderResources(0, 16, pSRV);
	Engine->GetContext()->GSSetShaderResources(0, 16, pSRV);
	Engine->GetContext()->HSSetShaderResources(0, 16, pSRV);
	Engine->GetContext()->DSSetShaderResources(0, 16, pSRV);
	Engine->GetContext()->CSSetShaderResources(0, 16, pSRV);
}

void PostProcessRender::PrePareRender()
{
	CreateSampler();

	ID3D11SamplerState* Samplers[] =
	{
		SamplerLinearWrap.Get(),
		SamplerLinearClamp.Get(),
		SamplerPointWrap.Get(),
		SamplerPointClamp.Get(),
		SamplerAnisoWrap.Get(),
		SamplerAnisoClamp.Get(),
		SamplerComparisonLinear.Get()
	};

	const UINT nSamplers = ARRAYSIZE(Samplers);

	Engine->GetContext()->VSSetSamplers(3, nSamplers, Samplers);
	Engine->GetContext()->PSSetSamplers(3, nSamplers, Samplers);
	//Engine->GetContext()->HSSetSamplers(3, nSamplers, Samplers);
	//Engine->GetContext()->DSSetSamplers(3, nSamplers, Samplers);
	Engine->GetContext()->CSSetSamplers(3, nSamplers, Samplers);


}

void PostProcessRender::CreateSampler()
{
	HRESULT hr;

	// Point Clamp
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	memset(&samplerDesc.BorderColor, 0, sizeof(samplerDesc.BorderColor));
	samplerDesc.MinLOD = -FLT_MAX;
	samplerDesc.MaxLOD = FLT_MAX;

	hr = Engine->GetDevice()->CreateSamplerState(&samplerDesc, SamplerPointClamp.GetAddressOf());
	DEBUG(hr);

	samplerDesc.AddressU = samplerDesc.AddressV = samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	hr = Engine->GetDevice()->CreateSamplerState(&samplerDesc, SamplerPointWrap.GetAddressOf());
	DEBUG(hr);

	samplerDesc.AddressU = samplerDesc.AddressV = samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

	hr = Engine->GetDevice()->CreateSamplerState(&samplerDesc, SamplerLinearClamp.GetAddressOf());
	DEBUG(hr);

	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	hr = Engine->GetDevice()->CreateSamplerState(&samplerDesc, SamplerLinearWrap.GetAddressOf());
	DEBUG(hr);

	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.MaxAnisotropy = 8;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0;
	samplerDesc.MipLODBias = -1.0f;
	hr = Engine->GetDevice()->CreateSamplerState(&samplerDesc, SamplerAnisoWrap.GetAddressOf());
	DEBUG(hr);

	samplerDesc.AddressU = samplerDesc.AddressV = samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

	hr = Engine->GetDevice()->CreateSamplerState(&samplerDesc, SamplerAnisoClamp.GetAddressOf());
	DEBUG(hr);

	samplerDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS;
	memset(&samplerDesc.BorderColor, 0, sizeof(samplerDesc.BorderColor));
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = 0;

	hr = Engine->GetDevice()->CreateSamplerState(&samplerDesc, SamplerComparisonLinear.GetAddressOf());
	DEBUG(hr);
}

void PostProcessRender::ConstantUpdate(ID3D11RenderTargetView* backbuffer, float Time, float DeltaTime, float Farz, float Near)
{
	this->Backbuffer = backbuffer;

	UINT EngineWidth = Engine->GetClientWidth();
	UINT EngineHeight = Engine->GetClientHeight();

	D3DXMATRIX Porj = Engine->GetProjectionMatrix();
	D3DXMATRIX ViewMat = Engine->GetCurrentCamera()->GetView();
	D3DXMATRIX ViewProj;
	D3DXMatrixMultiply(&ViewProj, &ViewMat, &Porj);

	D3DXMatrixInverse(&ViewProj, nullptr, &ViewProj);

	const float Width = static_cast<float>(RenderTargetColor.Width);
	const float Height = static_cast<float>(RenderTargetColor.Height);

	{
		D3DXMATRIX ViewPort = D3DXMATRIX(2.0f / Width, 0.0f, 0.0f, 0.0f,
			0.0f, -2.0f / Height, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			-1.0f, 1.0f, 0.0f, 1.0f);

		D3DXMatrixMultiply(&ViewProj, &ViewPort, &ViewProj);
	}

	auto& data = viewdata.GetData();
	data.InvViewProjection = ViewProj;
	auto pos = Engine->GetCurrentCamera()->GetEye();
	data.CamPos = D3DXVECTOR4(pos.x, pos.y, pos.z, 1.0f);
	data.viewport = D3DXVECTOR4(EngineWidth, EngineHeight, 1.0f / EngineWidth, 1.0f / EngineHeight);

	if (isReversedDepth)
	{
		data.DepthScaleFactor.x = -1.0f;
		data.DepthScaleFactor.y = 1.0f;
	}
	else
	{
		data.DepthScaleFactor.x = 1.0f;
		data.DepthScaleFactor.y = 0.0f;
	}

	data.CameraLearfar.y = Farz / 1000.0f;
	data.CameraLearfar.x = -data.CameraLearfar.y + ((Near * data.CameraLearfar.y) / Farz);

	auto& frame = framebuffer.GetData();
	frame.Time = DeltaTime * 0.01f;
	frame.TimeDelta = DeltaTime;

	auto& light = lightbuffer.GetData();
	D3DXVECTOR3 CamPos = Engine->GetCurrentCamera()->GetEye();
	light.lightDir = Engine->GetLight()->GetDirection();
	light.AmbientLight = D3DXVECTOR4(0.1f, 0.1f, 0.1f, 1.0f);
	D3DXVec3Normalize(&light.lightDir, &light.lightDir);

	D3DXMATRIX lightview;

	D3DXMatrixLookAtLH(&lightview, &CamPos, &light.lightDir, &D3DXVECTOR3(0.0f, 1.0f, 0.0f));

	D3DXMATRIX Orthproj;
	D3DXMatrixOrthoOffCenterLH(&Orthproj, 0, EngineWidth, 0, EngineHeight, -1, 1);

	D3DXMATRIX lightproj;
	D3DXMatrixMultiply(&lightproj, &lightview, &Orthproj);

	D3DXMatrixTranspose(&lightproj, &lightproj);

	light.ViewProjShadow = lightproj;
	light.SpecExp = 52.0f;
	light.LightColor = D3DXVECTOR3(0.25f, 0.25f, 0.25f);
}
 