
Texture2D InputTexture0 : register(t3);
Texture2D InputTexture1 : register(t4);
Texture2D InputTexture2 : register(t5);
Texture2D InputTexture3 : register(t6);

SamplerState samplerLinearWrap : register(s3);
SamplerState samplerLinearClamp : register(s4);
SamplerState samplerPointWrap : register(s5);
SamplerState samplerPointClamp : register(s6);
SamplerState samplerAnisoWrap : register(s7);
SamplerState samplerAnisoClamp : register(s8);
SamplerComparisonState samplerLinearComparison : register(s9);

static const float3 LUMINANCE_RGB = float3(0.2126, 0.7152, 0.0722);
static const float PI = 3.14159265;
static const float PI_INVERSE = 0.318309886;
static const float LUT_SCALE_16 = 0.9375;
static const float LUT_OFFSET_16 = 0.03125;

struct PostProcessOut
{
    float4 Position : SV_Position;
    float2 TextureUV : TEXCOORD0;
    float2 Params : ATTRIBUTE0;
};

cbuffer ViewData : register(b3)
{
    row_major matrix InvViewProjViewport;
    float4 CameraPos;
    float4 ViewPort;
    float2 cameraNearFar;
    float2 depthScaleFactors;
}

cbuffer LightBuffer : register(b4)
{
    row_major matrix ViewProjShadow;
    float3 lightDir;
    float Shininess;
    
    float4 ambientLight2;
    
    float3 LightColor;
    float pad2;
}

cbuffer cbPostProcess : register(b1)
{
    float4 TW3VignetteWihght;
    float4 TW3VignetteColor;
    float TW3VignetteOpacity;
    
    float3 Padding;
}

cbuffer FrameData : register(b7)
{
    float Time;
    float TimeDelta;
    float2 Pading;
};

cbuffer PointConst : register(b6)
{
    float2 InputSize0;
    float2 align;
}

float3 ColorCorrection(in Texture3D tex3DLUT, in float3 inColor)
{
    float3 lookup = saturate(inColor) * LUT_SCALE_16 + LUT_OFFSET_16;

    return tex3DLUT.SampleLevel(samplerLinearClamp, lookup, 0).rgb;
}



float3 GammaToLinear(in float3 clr)
{
    return pow(max(clr, float3(0, 0, 0)), 2.2f);
}

void ChromaticAberration(in Texture2D TexColorBuffer, float2 uv, in float fChromaticAbberationIntensity, inout float3 color)
{
	// User-defined params
    float2 chromaticAberrationCenter = float2(0.5, 0.5);
    float chromaticAberrationCenterAvoidanceDistance = 0.20; // chromatic aberration start
    float fA = 1.25; // chromatic aberration range

    float2 chromaticAberrationOffset = uv - chromaticAberrationCenter;
    chromaticAberrationOffset = chromaticAberrationOffset / chromaticAberrationCenter;

    float chromaticAberrationOffsetLength = length(chromaticAberrationOffset);

    float chromaticAberrationOffsetLengthFixed = chromaticAberrationOffsetLength - chromaticAberrationCenterAvoidanceDistance;
    float chromaticAberrationTexel = saturate(chromaticAberrationOffsetLengthFixed * fA);

    float fApplyChromaticAberration = (0.0 < chromaticAberrationTexel);
    if (fApplyChromaticAberration)
    {
        chromaticAberrationTexel *= chromaticAberrationTexel;
        chromaticAberrationTexel *= 0.50; // chromatic aberration size

        chromaticAberrationOffsetLength = max(chromaticAberrationOffsetLength, 1e-4);

        float fMultiplier = chromaticAberrationTexel / chromaticAberrationOffsetLength;

        chromaticAberrationOffset *= fMultiplier;
        chromaticAberrationOffset *= ViewPort.zw;
        chromaticAberrationOffset *= fChromaticAbberationIntensity;

        float2 offsetUV = -chromaticAberrationOffset * 2 + uv;
        color.r = TexColorBuffer.SampleLevel(samplerLinearClamp, offsetUV, 0).r;

        offsetUV = uv - chromaticAberrationOffset;
        color.g = TexColorBuffer.SampleLevel(samplerLinearClamp, offsetUV, 0).g;
    }
}

float3 LinearToGamma(in float3 clr)
{
    return pow(max(clr, float3(0, 0, 0)), 1.0f / 2.2f);
}