#include "Header.hlsli"
#include "PostProcessHeader.hlsli"

TextureCube TexCubemap : register(t0);
Texture2D TexDiffuse : register(t1);
Texture2D TexFalloff : register(t2);
Texture2D TexClouds : register(t3);
Texture2D TexPerlin : register(t4);
Texture2D TexNormals : register(t5);

Texture2D TexDepth : register(t10);

cbuffer cbClouds : register(b10)
{
    float4 cloudsParams0; // xy: uv scale, zw - move vector
    float4 cloudsParams1; // x: move speed, y: perlin scale, z: brightness, w: test alpha
}

struct VS_OUTPUT
{
    float4 PositionH : SV_POSITION;
    float3 PositionW : POSITION;
    float2 TextureUV : TEXCOORD;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float3 Bitangent : BITANGENT;
    float HeightRatio : CLOUDHEIGHT;
    float FarPlane : FARPLANE;
};

float AngleBetween(in float3 dir0, in float3 dir1)
{
    return acos(dot(dir0, dir1));
}

float3 ClearSky(in float3 dir, in float3 sunDir)
{
    float3 skyDir = float3(dir.x, abs(dir.y), dir.z);
    float gamma = AngleBetween(skyDir, sunDir);
    float S = AngleBetween(sunDir, float3(0, 1, 0));
    float theta = AngleBetween(skyDir, float3(0, 1, 0));

    float cosTheta = cos(theta);
    float cosS = cos(S);
    float cosGamma = cos(gamma);

    float num = (0.91f + 10 * exp(-3 * gamma) + 0.45 * cosGamma * cosGamma) * (1 - exp(-0.32f / cosTheta));
    float denom = (0.91f + 10 * exp(-3 * S) + 0.45 * cosS * cosS) * (1 - exp(-0.32f));

    float lum = num / max(denom, 0.0001f);

    const float3 SkyColor = float3(0.2, 0.5, 1.0f) * 1;
    const float3 SunColor = float3(1.0, 0.8f, 0.3f) * 15;
    const float SunWidth = 0.025f;
	
    float3 color = SkyColor;

	{
        float sunGamma = AngleBetween(dir, sunDir);
        color = lerp(SunColor, SkyColor, saturate(abs(sunGamma) / SunWidth));
    }

    return max(color * lum, 0);
}

float4 PS(in VS_OUTPUT Input) : SV_Target
{
	/* CIEClearSky Model */	
	
    //const float3 PositionW = normalize(Input.PositionW);
    //const float3 LightDirection = normalize(-lightDir);
    //float3 FinalColor = ClearSky(PositionW, LightDirection);
	
    //float Bias = 1;

    //return float4(FinalColor * Bias, 1);


	/* Private Research - Witcher 3 sky model */
    float3 PositionWorld = Input.PositionW;
    float3 vCameraToWorld = PositionWorld - CameraPos.xyz;
    float fDistanceCameraToWorld = length(vCameraToWorld);

    float3 vCameraToWorldNorm = vCameraToWorld / fDistanceCameraToWorld;

    float LightDotView = dot(normalize(lightDir), vCameraToWorldNorm);

    float LightDotViewX2 = abs(LightDotView) * abs(LightDotView);

    float length_param = saturate(15000 * 0.0002 - 0.3);
    LightDotViewX2 *= length_param;

    bool bResult = (LightDotView > 0.0);

    float3 fogColorFront = float3(4.32, 6.117, 8.54);
    float3 fogColorMiddle = float3(2.5, 3.86, 5.54);
    float3 fogColorBack = float3(1.39, 1.22, 1.891);


    float3 colorFogBackOrFront = (bResult) ? fogColorFront : fogColorBack;
    float3 fogColor = lerp(fogColorMiddle, colorFogBackOrFront, LightDotViewX2);

    float4 skyColor = float4(0.3636, 0.30663, 0.29765, 1); // 200
    float4 skyColorHorizon = float4(0.45364, 0.63371, 0.81951, 0.81851); // 201
    float4 sunColorHorizon = float4(1, 4.33, 0.94, 0); // cb4_v202
    float4 sunBackHorizonColor = float4(120.00, -0.95, 0.28, 0.05); // 203
    float4 sunColorSky = float4(22.85993, -0.95456, 0.28026, -0.10133); //204
    float4 moonColorHorizon = float4(1, 0.0209, 0, 0); // 205

    float3 vWorldToCamera = normalize(CameraPos.xyz - PositionWorld);

    float2 r0_xy = saturate(moonColorHorizon.yx);

    float worldToCamDot = length(-vWorldToCamera);
    worldToCamDot = 1.0 / worldToCamDot;

    vWorldToCamera = vWorldToCamera * worldToCamDot - vWorldToCamera;

    float2 sunColorSkyVector = normalize(sunColorSky.yz);

    float r2_x = saturate(dot(vWorldToCamera, sunColorSky.ywz));

    float r0_z = vCameraToWorld.y / fDistanceCameraToWorld;
    r0_z = r0_z * 1000 + CameraPos.y;
    r0_z += 710.0;
    r0_z *= sunColorHorizon.z;
    r0_z = r0_z * 0.001 + 0.1;
    r0_z = max(r0_z, 1e-4);
    r0_z = 1.0 / r0_z;
    r0_z = min(r0_z, 1);
    r0_z = pow(r0_z, 2.8);
    r0_z = -r0_z + 1;

    float3 finalColor = lerp(skyColor.rgb, fogColor.rgb, r0_z);

    return float4(fogColor.rgb, 1);
}