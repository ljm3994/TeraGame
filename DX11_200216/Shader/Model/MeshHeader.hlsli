#include "../Header.hlsli"

static const uint NumCascades = 4;

Texture2DArray ShadowMap : register(t3);
TextureCube<float3> SpecularCubemap : register(t4);
Texture2D<float2> SpecularCubemapLookup : register(t5);

SamplerState AnisoSampler : register(s3);
SamplerState EVSMSampler : register(s4);
SamplerState LinearSampler : register(s5);

static const float PI = 3.14159265;

cbuffer ModelConstants : register(b3)
{
    float3 CameraPos;
    float pad;
    float4x4 ShadowMatrix;
    float4 CascadeSplits;
    float4 CascadeOffsets[NumCascades];
    float4 CascadeScales[NumCascades];
    float OffsetScale;
    float PositiveExponent;
    float NegativeExponent;
    float LightBleedingReduction;
    float3 EnvironmentSH[9];
    float2 RTSize;
    float2 JitterOffset;
}

cbuffer SettingsValue : register(b7)
{
    int MSAAMode;
    int ResolveFilterType;
    float ResolveFilterDiameter;
    float GaussianSigma;
    float CubicB;
    float CubicC;
    bool UseStandardResolve;
    bool InverseLuminanceFiltering;
    bool UseExposureFiltering;
    float ExposureFilterOffset;
    bool UseGradientMipLevel;
    bool EnableTemporalAA;
    float TemporalAABlendFactor;
    bool UseTemporalColorWeighting;
    int NeighborhoodClampMode;
    float VarianceClipGamma;
    float LowFreqWeight;
    float HiFreqWeight;
    float SharpeningAmount;
    int DilationMode;
    float MipBias;
    int ReprojectionFilter;
    bool UseStandardReprojection;
    int CurrentScene;
    float3 LightDirection;
    float3 LightColor;
    bool EnableDirectLighting;
    bool EnableAmbientLighting;
    bool RenderBackground;
    bool EnableShadows;
    bool EnableNormalMaps;
    float NormalMapIntensity;
    float DiffuseIntensity;
    float Roughness;
    float SpecularIntensity;
    float4 ModelOrientation;
    float ModelRotationSpeed;
    bool DoubleSyncInterval;
    float ExposureScale;
    bool EnableZoom;
    float BloomExposure;
    float BloomMagnitude;
    float BloomBlurSigma;
    float ManualExposure;
}

struct PS_OUT
{
    float4 Color : SV_Target0;
    float2 Velocity : SV_Target1;
};

float GGX_V1(in float m2, in float nDotX)
{
    return 1.0f / (nDotX + sqrt(m2 + (1 - m2) * nDotX * nDotX));
}

float3 Fresnel(in float3 specAlbedo, in float3 h, in float3 l)
{
    float3 fresnel = specAlbedo + (1.0f - specAlbedo) * pow((1.0f - saturate(dot(l, h))), 5.0f);

    // Fade out spec entirely when lower than 0.1% albedo
    fresnel *= saturate(dot(specAlbedo, 333.0f));

    return fresnel;
}

float GGX_Specular(in float m, in float3 n, in float3 h, in float3 v, in float3 l)
{
    float nDotH = saturate(dot(n, h));
    float nDotL = saturate(dot(n, l));
    float nDotV = saturate(dot(n, v));

    float nDotH2 = nDotH * nDotH;
    float m2 = m * m;

    // Calculate the distribution term
    float d = m2 / (PI * pow(nDotH * nDotH * (m2 - 1) + 1, 2.0f));

    // Calculate the matching visibility term
    float v1i = GGX_V1(m2, nDotL);
    float v1o = GGX_V1(m2, nDotV);
    float vis = v1i * v1o;

    return d * vis;
}

float3 CalcLighting(in float3 normal, in float3 lightDir, in float3 lightColor,
					in float3 diffuseAlbedo, in float3 specularAlbedo, in float roughness,
					in float3 positionWS)
{
    float3 lighting = diffuseAlbedo * (1.0f / 3.14159f);

    float3 view = normalize(CameraPos - positionWS);
    const float nDotL = saturate(dot(normal, lightDir));
    if (nDotL > 0.0f)
    {
        const float nDotV = saturate(dot(normal, view));
        float3 h = normalize(view + lightDir);

        float specular = GGX_Specular(roughness, normal, h, view, lightDir);
        float3 fresnel = Fresnel(specularAlbedo, h, lightDir);

        lighting += specular * fresnel;
    }

    return lighting * nDotL * lightColor;
}

float3 EvalCosine(float3 normal, float3 color[9])
{
    float dirSH[9];
    
    dirSH[0] = 0.282095f;
    
    dirSH[1] = 0.488603f * normal.y;
    dirSH[2] = 0.488603f * normal.z;
    dirSH[3] = 0.488603f * normal.x;
    
    dirSH[4] = 1.092548f * normal.x * normal.y;
    dirSH[5] = 1.092548f * normal.y * normal.z;
    dirSH[6] = 0.315392f * (3.0f * normal.z * normal.z - 1.0f);
    dirSH[7] = 1.092548f * normal.x * normal.z;
    dirSH[8] = 0.546274f * (normal.x * normal.x - normal.y * normal.y);
    
    dirSH[0] *= 1.0f;
    dirSH[1] *= (2.0f / 3.0f);
    dirSH[2] *= (2.0f / 3.0f);
    dirSH[3] *= (2.0f / 3.0f);
    dirSH[4] *= 0.25f;
    dirSH[5] *= 0.25f;
    dirSH[6] *= 0.25f;
    dirSH[7] *= 0.25f;
    dirSH[8] *= 0.25f;

    float3 result = 0;
    
    [unroll]
    for (int i = 0; i < 9; i++)
    {
        result += dirSH[i] * color[i];
    }

    return result;
}

float2 GetEVSMExponents(in float positiveExponent, in float negativeExponent,
                        in float3 cascadeScale)
{

    const float maxExponent = 42.0f;

    float2 lightSpaceExponents = float2(positiveExponent, negativeExponent);

    return min(lightSpaceExponents / cascadeScale.z, maxExponent);
}

float2 WarpDepth(float depth, float2 exponents)
{
    depth = 2.0f * depth - 1.0f;
    float pos = exp(exponents.x * depth);
    float neg = -exp(-exponents.y * depth);
    return float2(pos, neg);
}

float Linstep(float a, float b, float v)
{
    return saturate((v - a) / (b - a));
}

float ReduceLightBleeding(float pMax, float amount)
{
    return Linstep(amount, 1.0f, pMax);
}


float ChebyshevUpperBound(float2 moments, float mean, float minVariance,
                          float lightBleedingReduction)
{
    float variance = moments.y - (moments.x * moments.x);
    variance = max(variance, minVariance);

    float d = mean - moments.x;
    float pMax = variance / (variance + (d * d));

    pMax = ReduceLightBleeding(pMax, lightBleedingReduction);

    return (mean <= moments.x ? 1.0f : pMax);
}

float SampleShadowMapEVSM(in float3 shadowPos, in float3 shadowPosDX,
                          in float3 shadowPosDY, uint cascadeIdx)
{
    float2 exponents = GetEVSMExponents(PositiveExponent, NegativeExponent,
                                        CascadeScales[cascadeIdx].xyz);
    float2 warpedDepth = WarpDepth(shadowPos.z, exponents);

    float4 occluder = ShadowMap.SampleGrad(EVSMSampler, float3(shadowPos.xy, cascadeIdx),
                                            shadowPosDX.xy, shadowPosDY.xy);

    float2 depthScale = 0.0001f * exponents * warpedDepth;
    float2 minVariance = depthScale * depthScale;

    float posContrib = ChebyshevUpperBound(occluder.xz, warpedDepth.x, minVariance.x, LightBleedingReduction);
    float negContrib = ChebyshevUpperBound(occluder.yw, warpedDepth.y, minVariance.y, LightBleedingReduction);
    float shadowContrib = posContrib;
    shadowContrib = min(shadowContrib, negContrib);

    return shadowContrib;
}

float SampleShadowCascade(in float3 shadowPosition, in float3 shadowPosDX,
                          in float3 shadowPosDY, in uint cascadeIdx)
{
    shadowPosition += CascadeOffsets[cascadeIdx].xyz;
    shadowPosition *= CascadeScales[cascadeIdx].xyz;

    shadowPosDX *= CascadeScales[cascadeIdx].xyz;
    shadowPosDY *= CascadeScales[cascadeIdx].xyz;

    float shadow = SampleShadowMapEVSM(shadowPosition, shadowPosDX, shadowPosDY, cascadeIdx);

    return shadow;
}

float ShadowVisibility(in float3 positionWS, in float depthVS)
{
    float shadowVisibility = 1.0f;
    uint cascadeIdx = 0;

    // Project into shadow space
    float3 samplePos = positionWS;
    float3 shadowPosition = mul(float4(samplePos, 1.0f), ShadowMatrix).xyz;
    float3 shadowPosDX = ddx(shadowPosition);
    float3 shadowPosDY = ddy(shadowPosition);

	// Figure out which cascade to sample from
	[unroll]
    for (uint i = 0; i < NumCascades - 1; ++i)
    {
		[flatten]
        if (depthVS > CascadeSplits[i])
            cascadeIdx = i + 1;
    }

    shadowVisibility = SampleShadowCascade(shadowPosition, shadowPosDX, shadowPosDY,
                                           cascadeIdx);

    return shadowVisibility;
}