#include "PostProcessHeader.hlsli"

Texture2D TexDepth : register(t3);
Texture2D RT0 : register(t4);
Texture2D RT1 : register(t5);
Texture2D RT2 : register(t6);
Texture2D TexHBAOPlus : register(t7);
//Texture2D ShadowMap : register(t8);

struct GBufferData
{
    float3 Albedo;
    float Roughness;

    float3 Normal;
    float4 Specualr;
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

    Output.Roughness = 0.5f;

    Output.Normal = RT1.Load(location).xyz;
    Output.Normal = normal_decode(Output.Normal);

    Output.Metallic = rt2.x;
    Output.Specualr = rt2;
    Output.MaterialID = (uint) rt2.y;

    float fDepthBufferDepth = TexDepth.Load(location).x;
    float4 vWSPosition = mul(float4((float) location.x + 0.5f, (float) location.y + 0.5f, fDepthBufferDepth, 1.0f), InvViewProjViewport);
    Output.WorldPos = vWSPosition.xyz / vWSPosition.w;

    Output.AmbientOcclusion = TexHBAOPlus.Load(location).x;
	
    return Output;
}

static const float RCP_SHADOWMAP = 1.0 / 2048.0;

float2 texOffset(int u, int v)
{
    const float shadowMapSize = 2048;
    return float2(u * RCP_SHADOWMAP, v * RCP_SHADOWMAP);
}

float4 PS(PostProcessOut Input) : SV_TARGET0
{
    int3 location = int3(Input.Position.xy, 0);
	
    GBufferData GBuffer = DecodeGBuffer(location);
    float fDepthBufferDepth = TexDepth.Load(location).x;
    fDepthBufferDepth = fDepthBufferDepth * depthScaleFactors.x + depthScaleFactors.y;

    float3 FinalColor = 0;

    if (fDepthBufferDepth < 1.0)
    {
        float3 Albedo = GBuffer.Albedo;
		
        float AO = GBuffer.AmbientOcclusion;

        float3 N = GBuffer.Normal;

        float3 L = normalize(lightDir);

        float diffuseIntensity = saturate(dot(N, -L));

        float3 V = normalize(GBuffer.WorldPos - CameraPos.xyz);

        float specularIntensity = 0;
        
        if (diffuseIntensity > 0)
        {
            float3 halfWay = normalize(V + L);
            specularIntensity = saturate(dot(-halfWay, N));
            
            specularIntensity = pow(specularIntensity, 8.0f);
        }
        
        //float NdotL = max(1e-5, dot(N, L));
	

        //float Roughness = GBuffer.Roughness;
        //float Metallic = GBuffer.Metallic;
        //float3 SpecularColor = lerp(0.04, Albedo.rgb, Metallic);

        float3 ambientUp = Albedo * 0.3;
        float3 ambientDown = ambientUp * 0.5;

        float fAmbientBlend = 0.5 * N.y + 0.5;
        float3 Ambient = ambientUp * fAmbientBlend + ambientDown * (1.0 - fAmbientBlend);
        Ambient = Ambient * AO * float3(0.5, 0.55, 0.7);

        //float4 lightSpacePos = mul(float4(GBuffer.WorldPos, 1.0), ViewProjShadow);
        //lightSpacePos.xyz /= lightSpacePos.w;
        //
        //lightSpacePos.xy = lightSpacePos.xy * float2(0.5, -0.5) + 0.5;
        //
		//// 3) Apply bias
        //lightSpacePos.z -= 0.003;

		// 4) PCF 4x4 sampling
//   float sum = 0.0;
//
//[unroll]
//   for (float y = -1.5; y <= 1.5; y += 1.0)
//   {
//	   [unroll]
//       for (float x = -1.5; x <= 1.5; x += 1.0)
//       {
//           sum += ShadowMap.SampleCmpLevelZero(samplerLinearComparison, lightSpacePos.xy + texOffset(x, y), lightSpacePos.z);
//       }
//   }
//
        float shadowFactor = 20.0f / 16.0f; //sum / 16.0;

        //float NdotH = max(0.0, dot(N, H));

        float specEnergyConservation = (8.0 + 32.0f) / (8 * PI);
        float3 specular = specularIntensity * GBuffer.Specualr; //pow(NdotH, Shininess) * specEnergyConservation;

        FinalColor = (Albedo * AO * diffuseIntensity) + specular;
        FinalColor += (ambientLight2.xyz * Albedo);
        FinalColor = lerp(Ambient, FinalColor, shadowFactor);
    }
	
    return float4(FinalColor, 1.0f) * 10.0f;
}