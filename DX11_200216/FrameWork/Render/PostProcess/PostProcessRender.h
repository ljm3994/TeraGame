#pragma once
#include "FrameWork\Render\PostProcess\PostGraphic\FXAA.h"
#include "FrameWork\DXTypes\RenderTarget2D.h"
#include "FrameWork\Buffer\PostProcess\ViewDataBuffer.h"
#include <FrameWork\States\DepthStencilState.h>
#include "FrameWork\Buffer\MRT\GBuffer.h"
#include <FrameWork\Render\PostProcess\HBAO\HBAOPlus.h>
#include "FrameWork\Render\PostProcess\FullScreenPass.h"
#include "FrameWork\Render\PostProcess\PostGraphic\AverageLuminance.h"
#include "FrameWork\Render\PostProcess\PostGraphic\LensFlares.h"
#include "FrameWork\States\RasterizerState.h"
#include "FrameWork\Render\PostProcess\PostGraphic\Sharpen.h"
#include "FrameWork\States\BlendState.h"
#include <Object\Sky.h>

class SkyBox;

class PostProcessRender
{
	struct ToneMapping
	{
		D3DXVECTOR4 CurveABCD;
		D3DXVECTOR2 CurveEF;
		D3DXVECTOR2 MinMaxAllowedSceneLuminance;
		float WhitPoint;
		float NominatorMult;
		float powParam;
		float zPadding;
	};

	struct PostProcessBuffer
	{
		D3DXVECTOR4 VignetteWeight;
		D3DXVECTOR4 VignetteColor;
		float VigentteOpacity;
		float padding[3];

		PostProcessBuffer()
		{
			VignetteWeight = D3DXVECTOR4(250.0f / 255.0f, 250.0f / 255.0f, 250.0f / 255.0f, 250.0f / 255.0f);
			VignetteColor = D3DXVECTOR4(2.0f / 255.0f, 2.0f / 255.0f, 2.0f / 255.0f, 2.0f / 255.0f);
			VigentteOpacity = 0.65f;
		}
	};

	struct PostProcessViewData
	{
		D3DXMATRIX InvViewProjection;
		D3DXVECTOR4 CamPos;
		D3DXVECTOR4 viewport;
		D3DXVECTOR2 CameraLearfar;
		D3DXVECTOR2 DepthScaleFactor;
	};

	struct FramData
	{
		float Time;
		float TimeDelta;
		float Pading[2];
	};

	struct LightBuffer
	{
		D3DXMATRIX ViewProjShadow;
		D3DXVECTOR3 lightDir;
		float SpecExp;
		D3DXVECTOR3 LightColor;
		D3DXVECTOR4 AmbientLight;
		float pad2;
	};

	ConstantBuffer< LightBuffer> lightbuffer;
	ConstantBuffer< FramData> framebuffer;
	ConstantBuffer< PostProcessViewData> viewdata;
	ConstantBuffer<PostProcessBuffer> postbuffer;
	ToneMapping toneMappingparams;
	ConstantBuffer< ToneMapping> toneMapping;
	FXAA* fxaa;
	D3D11_VIEWPORT vp;

	ID3D11RenderTargetView* Backbuffer;
	RenderTarget2D  RenderTargetColor;
	RenderTarget2D	RenderTargetFXAAProxy;
	RenderTarget2D	BloomTexHalfRes;
	RenderTarget2D bloomTex[2];
	RenderTarget2D motionBlurTex;
	DepthStencilBuffer ShadowMap;
	
	Sky* sky;

	ComPtr<ID3D11SamplerState> SamplerPointWrap;
	ComPtr<ID3D11SamplerState> SamplerPointClamp;
	ComPtr<ID3D11SamplerState> SamplerLinearWrap;
	ComPtr<ID3D11SamplerState> SamplerLinearClamp;
	ComPtr<ID3D11SamplerState> SamplerAnisoWrap;
	ComPtr<ID3D11SamplerState> SamplerAnisoClamp;
	ComPtr<ID3D11SamplerState> SamplerComparisonLinear;

	D3DXMATRIX view;
	D3DXMATRIX orthographic;

	GBuffer* gbuffer;
	DepthStencilState* depthState[3];
	RasterizeState* OrginalRs;
	RasterizeState* resterCullBackFace;
	RasterizeState* resterCullMSAA;
	RasterizeState* resterCullNo;

	BlendState * blend[2];
	BlendState* bloomBlendState;
	ComPtr<ID3D11Texture2D> dsvTexture;
	ComPtr<ID3D11DepthStencilView> dsv;

	HBAOPlus hbaoplus;
	HBAOParam hbaoParam;

	Shader* DefferedPSShader;
	Shader* DefferedCSShader;
	Shader* BloomPSThreshold;
	Shader* BloomPSShaderBlurH;
	Shader* BloomPSShaderBlurV;
	Shader* PSShaderToneMapping;
	Shader* MotionBlurPSShader;
	Shader* SkyboxVelocity;

	Shader* PSShaderFinalPostProcess;
	Shader* PSShaderFinalPostProcessShowVignette;
	FullScreenPass fullscreenpass;
	AverageLuminance averageluminance;
	LensFlares lensflares;
	Sharpen sharpen;

	Texture* textureLUT;
	Texture* texLensDirt;
	Texture* texLensStar;
	Texture* textureNoise;
	Texture* textureVignetteMask;
	Texture* texturenormalFit;

	unique_ptr<DirectX::BasicPostProcess> basicpostprocess;
	unique_ptr<DirectX::ToneMapPostProcess> Tonepostprocess;
	unique_ptr<DirectX::DualPostProcess> Dualpostprocess;
	//DepthStencilState depthstate[2];
	bool isReversedDepth = false;
	bool isClearGBuffer = true;
	bool isDefrredShadingUseComputeShader = false;
	bool isShowVignette = false;
	bool isDebugView = false;
	bool isFirst = true;
public:
	PostProcessRender();
	~PostProcessRender();
	void Init(ID3D11RenderTargetView* backbuffer);
	void Update(float DeltaTime);
	void Set();
	void Render();
	void PrePareRender();
	void ConstantUpdate(ID3D11RenderTargetView* backbuffer, float Time, float DeltaTime, float Farz = 1000.0f, float Near = 0.1f);
private:
	void CreateSampler();
};