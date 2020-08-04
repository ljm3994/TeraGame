#include "PostProcessHeader.hlsli"
#include "FAXXHeader.hlsli"

Texture2D TexInput0 : register(t3);

#define USE_FXAA				1

#define FXAA_PC					1
#define FXAA_HLSL_5				1
#define FXAA_QUALITY__PRESET	23

float4 PS(PostProcessOut Input) : SV_TARGET
{
    FxaaTex InputFXAATex = { samplerAnisoClamp, TexInput0 };

    float4 FXAAColor = FxaaPixelShader(
		Input.TextureUV.xy,
		FxaaFloat4(0.0f, 0.0f, 0.0f, 0.0f), 
		InputFXAATex, 
		InputFXAATex, 
		InputFXAATex, 
		ViewPort.zw,
		FxaaFloat4(0.0f, 0.0f, 0.0f, 0.0f), 
		FxaaFloat4(0.0f, 0.0f, 0.0f, 0.0f), 
		FxaaFloat4(0.0f, 0.0f, 0.0f, 0.0f), 
		0.75f, 
		0.166f, 
		0.0833f, 
		0.0f, 
		0.0f, 
		0.0f, 
		FxaaFloat4(0.0f, 0.0f, 0.0f, 0.0f)
		);

    return FXAAColor;
}
