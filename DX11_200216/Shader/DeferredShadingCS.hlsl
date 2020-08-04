#include "PostProcessHeader.hlsli"

Texture2D<float4> TexDepth : register(t3);
Texture2D<float4> RT0 : register(t4);
Texture2D<float4> RT1 : register(t5);
Texture2D<float4> RT2 : register(t6);
Texture2D<float4> TexHBAOPlus : register(t7);
Texture2D<float4> ShadowMap : register(t8);

RWTexture2D<float4> TexOutput : register(u0);

#define TILE_SIZE	16

struct GBufferData
{
    float3 Albedo;
    float Roughness;

    float3 Normal;
    float Metallic;

    float3 WorldPos;
    uint MaterialID;

    float AmbientOcclusion;
};

float3 normal_decode(in float3 enc)
{
    return normalize(enc * 2.0 - 1.0);
}


GBufferData DecodeGBuffer(int3 location)
{
    GBufferData Output = (GBufferData) 0;


	// Read raw data from GBuffer
    float4 rt0 = RT0.Load(location);
    float4 rt2 = RT2.Load(location);

	// And write it to output
    Output.Albedo = (rt0.xyz);
    Output.Albedo = GammaToLinear(Output.Albedo);

    Output.Roughness = rt0.w;

    Output.Normal = RT1.Load(location).xyz;
	//Output.Normal = Output.Normal * 2.f - 1.f;
    Output.Normal = normal_decode(Output.Normal);

    Output.Metallic = rt2.x;
    Output.MaterialID = (uint) rt2.y;

	// Determine world position
    float fDepthBufferDepth = TexDepth.Load(location).x;
    float4 vWSPosition = mul(float4((float) location.x + 0.5f, (float) location.y + 0.5f, fDepthBufferDepth, 1.0f), InvViewProjViewport);
    Output.WorldPos = vWSPosition.xyz / vWSPosition.w;

	// Ambient Occlusion
    Output.AmbientOcclusion = TexHBAOPlus.Load(location).x;

    return Output;
}

float2 texOffset(int u, int v)
{
    const float shadowMapSize = 2048;
    return float2(u * 1.0f / shadowMapSize, v * 1.0f / shadowMapSize);
}

[numthreads(TILE_SIZE, TILE_SIZE, 1)]
void CS(uint3 groupID : SV_GroupID, uint3 groupThreadID : SV_GroupThreadID)
{
    uint2 positionXY = groupID.xy * uint2(TILE_SIZE, TILE_SIZE) + groupThreadID.xy;
    uint3 position = uint3(positionXY, 0);

    float fDepthBufferDepth = TexDepth.Load(position).x;
    fDepthBufferDepth = fDepthBufferDepth * depthScaleFactors.x + depthScaleFactors.y;

    float3 FinalColor = 0;

    if (fDepthBufferDepth < 1.0)
    {
        GBufferData GBuffer = DecodeGBuffer(position);
		
        float3 Albedo = GBuffer.Albedo;
        float AO = GBuffer.AmbientOcclusion;
        float3 N = GBuffer.Normal;

        float3 L = normalize(-lightDir);
        float3 H = normalize(N + L);
        float3 V = normalize(CameraPos.xyz - GBuffer.WorldPos);

        float NdotL = max(1e-5, dot(N, L));

        float Roughness = GBuffer.Roughness;
        float Metallic = GBuffer.Metallic;
        float3 SpecularColor = lerp(0.04, Albedo.rgb, Metallic);

        float3 realAlbedo = Albedo * (1.f - Metallic);
        realAlbedo *= (1.0 / PI);

		// Poor man's ambient cubemap
        float3 ambientUp = Albedo * 0.15;
        float3 ambientDown = ambientUp * 0.5;

        float fAmbientBlend = 0.5 * N.y + 0.5;
        float3 Ambient = ambientUp * fAmbientBlend + ambientDown * (1.0 - fAmbientBlend);
        Ambient = Ambient * AO * float3(0.5, 0.55, 0.7);


		// --------------------------------
		// Shadows
		// Single shadow map
		// --------------------------------
		// 1) Re-homogenize position after interpolation (Todo: Research, why?)
        float4 lightSpacePos = mul(float4(GBuffer.WorldPos, 1.0), ViewProjShadow);
        lightSpacePos.xyz /= lightSpacePos.w;

        lightSpacePos.xy = lightSpacePos.xy * float2(0.5, -0.5) + 0.5;

		// 3) Apply bias
        lightSpacePos.z -= 0.003;

		// 4) PCF 4x4 sampling
        float sum = 0.0;
		[unroll]
        for (float y = -1.5; y <= 1.5; y += 1.0)
        {
			[unroll]
            for (float x = -1.5; x <= 1.5; x += 1.0)
            {
                sum += ShadowMap.SampleCmpLevelZero(samplerLinearComparison, lightSpacePos.xy + texOffset(x, y), lightSpacePos.z);
            }
        }

        float shadowFactor = sum / 16.0;

		// Simple specular
        float3 reflLight = -reflect(L, N);
        float NdotH = max(0.0, dot(N, H));
        float specular = pow(NdotH, 32);

        float3 LightColor = 2.5 * float3(1.0, 0.85, 0.6);


        FinalColor = LightColor * Albedo * AO * NdotL + specular * NdotL * Albedo;
        FinalColor += Ambient;

        FinalColor = lerp(Ambient, FinalColor, shadowFactor);
        FinalColor = max(FinalColor, 0);
    }

    TexOutput[positionXY] = float4(FinalColor, 1);
}