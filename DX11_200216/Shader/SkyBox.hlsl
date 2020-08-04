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

cbuffer SelectEnv : register(b11)
{
    int isUseEnv;
    float3 padclude;
}

struct VertexUVNormalTangentModelInstance
{
    float4 position : POSITION;
    float2 uv : UV;
    float3 normal : NORMAL;
    float3 Tangent : TANGENT;
};

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

VS_OUTPUT VS(in VertexUVNormalTangentModelInstance Input)
{
    VS_OUTPUT Output;

    Output.PositionH = mul(Input.position, world);
    Output.PositionW = Output.PositionH.xyz;
    Output.PositionH = mul(Output.PositionH, view).xyzw;
    Output.PositionH = mul(Output.PositionH, projection).xyzw;
	
    Output.TextureUV = Input.uv;
    Output.HeightRatio = pow((Input.position.y * 0.0013), 3);

    Output.Bitangent = normalize(cross(Input.normal, Input.Tangent));
    Output.Normal = normalize(Input.normal);
    Output.Tangent = normalize(Input.Tangent);

    Output.FarPlane = Output.PositionH.w;
    
    return Output;
}

float4 PS(in VS_OUTPUT Input) : SV_Target
{
    if(isUseEnv == 1)
    {
        float3 ColorDiffuse = TexCubemap.SampleLevel(samplerLinearWrap, Input.PositionW * float3(1, -1, 1), 0).rgb;
        ColorDiffuse = GammaToLinear(ColorDiffuse);
        
        return float4(ColorDiffuse.rgb, 1);
    }
    
    float3 ColorDiffuse = TexDiffuse.Sample(samplerLinearWrap, Input.TextureUV).rgb;
    ColorDiffuse += float3(0.03, 0.11, 0.7) * 0.2;
    ColorDiffuse = GammaToLinear(ColorDiffuse);
    
    return float4(ColorDiffuse.rgb, 1);
}

float4 Cloude2(in VS_OUTPUT Input) : SV_Target
{
    float2 uvPerlin = Time * float2(-0.0005, 0.000);
    float2 texcoordsPerlin = Input.TextureUV * float2(0.5, 0.5) + uvPerlin;
    float xNoise = TexPerlin.Sample(samplerAnisoWrap, texcoordsPerlin).x;
    xNoise = xNoise * 1.0 - 1.0;
    xNoise = saturate(1 + xNoise);
    xNoise = saturate(xNoise);
    
    float2 uv = Time * float2(0.12, 0.04);
    float2 texcoords = Input.TextureUV * float2(0.75, 0.75) + uv;
	
    float4 colorClouds = TexClouds.Sample(samplerLinearWrap, texcoords + xNoise * 0.05).xxxx;
    colorClouds = pow(colorClouds, 2.2);
    
    float2 normals = TexNormals.Sample(samplerLinearWrap, texcoords).xy;
    normals = normals * 2.0 - 1.0;

    float normals_z = -normals.x * normals.x + 1.0;
    normals_z = -normals.y * normals.y + normals_z;
    normals_z = max(0, normals_z);

    float3 vNorm = float3(normals.x, normals_z, normals.y);

    vNorm = normalize(vNorm);

    float2 lightVector2D = float2(lightDir.x, lightDir.z);
    lightVector2D = normalize(lightVector2D);
    
    float3 worldToCameraXYZ = CameraPos.xyz - Input.PositionW;
    float worldToCameraDistanceXYZ = length(worldToCameraXYZ);

    float2 worldToCameraXZ = float2(worldToCameraXYZ.x, worldToCameraXYZ.z);
    worldToCameraXZ = normalize(worldToCameraXZ);
    worldToCameraXYZ /= worldToCameraDistanceXYZ;
    
    float LdotV = saturate(dot(worldToCameraXZ, lightVector2D));

    float NdotL = saturate(dot(-lightDir, vNorm));

    float Depth = TexDepth.Load(uint3(Input.PositionH.xy, 0)).r;
    Depth = Depth * depthScaleFactors.x + depthScaleFactors.y;
    Depth = Depth * cameraNearFar.x + cameraNearFar.y;
    Depth = max(Depth, 0.0001);

    float FarPlane = 1.0 / Depth;

    float SkyboxFarPlane = Input.FarPlane;
    FarPlane -= SkyboxFarPlane;

    FarPlane = saturate(FarPlane * 0.0001);

    float factor = (colorClouds.x);

    float3 colorBase = colorClouds.xyz;
    float3 colorDarken = colorClouds.rgb * NdotL * LdotV;

    float xFactor = dot(vNorm.xz, lightDir.xz);
    xFactor = saturate(xFactor);

    float3 colorLerped = lerp(colorBase, colorDarken, xFactor);

    return float4(colorLerped, 1.0f);
}

float4 Cloude1(in VS_OUTPUT Input) : SV_Target
{
    float2 translation = Time * cloudsParams0.zw;

    float2 perturbValue = TexPerlin.SampleLevel(samplerAnisoWrap, Input.TextureUV, 0).xy;
    perturbValue *= 0.02;
    
    float2 uv = Time * 1 * float2(0.2, 0.01);
    uv = Input.TextureUV.xy + uv;

    float4 colorClouds = TexClouds.Sample(samplerAnisoWrap, uv);

    float3 vNormal = colorClouds.xyz - 0.5;
    vNormal *= float3(0.25f, 1.f, 0.25);
    vNormal = normalize(vNormal * 2);


    float3x3 TBNMatrix = float3x3(Input.Tangent, Input.Bitangent, Input.Normal);
    vNormal = normalize(mul(vNormal, TBNMatrix));

    float3 vInvSunDir = lightDir * float3(1, -1, 1);
    vInvSunDir = normalize(vInvSunDir);

    float NormalDotInvSunDir = saturate(dot(vNormal, vInvSunDir));

    float4 cb4_v5 = float4(1, 1, 1, 1);
    float4 cb4_v6 = float4(3, 3, 3, 3);
    float4 cb4_v7 = float4(3, 3, 3, 3);
    float4 cb4_v8 = float4(1.25, 1.25, 1.25, 1.25);
    float4 cb4_v9 = float4(1, 1, 1, 1);
    float4 cb4_v10 = float4(0.5, 0.5, 0.5, 0.5);
    float4 cb4_v11 = float4(1.5, 1.5, 1.5, 1.5);

    float param = cb4_v7.x - cb4_v6.x;
    param = NormalDotInvSunDir * param + cb4_v6.x;
    param = clamp(param, 0.0, 5.0);
    param -= cb4_v5.x;

    float3 InvSunDir = normalize(lightDir);

    float3 SkyboxWorldPosition = Input.PositionW;
    float SkyboxFarPlane = Input.FarPlane;

    float3 cameraToWorld = CameraPos.xyz - SkyboxWorldPosition;
    float cameraToWorldDistance = length(cameraToWorld);
    cameraToWorld = normalize(cameraToWorld);

    float ViewDotLight = saturate(dot(cameraToWorld, vInvSunDir));

    float r0_y = ViewDotLight * param + cb4_v5.x;
    float r1_y = cb4_v9.x - cb4_v8.x;
    r1_y = ViewDotLight * r1_y + cb4_v8.x;

    r1_y = r1_y - r0_y;

    float r1_z = saturate(lightDir.y * 4 - 1);
    r0_y = r1_z * r1_y + r0_y;

    r1_y = cb4_v11.x - cb4_v10.x;
    r1_y = NormalDotInvSunDir * r1_y + cb4_v10.x;

    r0_y = r0_y * r1_y;
    r0_y = pow(r0_y, 2.2);

    float2 xzSunLight = normalize(lightDir.xz);

	// line 49
    float ViewDotLightXZ = dot(xzSunLight, cameraToWorld.xz);

	// line 50
    float3 cb12_v233 = float3(1.374, 1.84, 2.07);
    float3 cb12_v266 = float3(1.6, 0.71, 1.09);

	// r2.xyz
    float3 atmoColor = cb12_v233 - cb12_v266;
    atmoColor = NormalDotInvSunDir * atmoColor + cb12_v266;

    float r0_x = cameraToWorldDistance * 0.00667;
    r0_x = min(r0_x, 1);

	// line 54
    float3 cb12_v267 = float3(1.0832, 1.0832, 1.0832) * 0.2;
    float3 costam = float3(1, 1, 1);
    float3 atmoColor2 = cb12_v267 - costam;
    atmoColor2 = r0_x * atmoColor2 + costam;
    atmoColor2 = atmoColor2 * atmoColor - atmoColor;

    float3 atmoColor3 = ViewDotLightXZ * atmoColor2 + atmoColor;
    atmoColor3 *= r0_y;
	

	// line 59
    float3 cb12_v237 = float3(1.4582, 0.865, 0.27);
    atmoColor = -cb12_v237 * atmoColor3 + float3(1, 1.13, 0.55);
	

	// line 60
    atmoColor3 = atmoColor3 * cb12_v237;
	
    atmoColor *= (clamp(0, 1, 1 - Input.HeightRatio));

	// Line 62!
    r0_x = cameraToWorldDistance - 0; // -cb4_v0.x;
    r0_y = cameraToWorldDistance - 0; // -cb12_v0.x;
    float r0_z = 10;
    r0_x = saturate(r0_x / r0_z);

	// line 66
    float3 foo = r0_x * atmoColor + atmoColor3;
    foo *= 0.61195; // cb2_v2
	

	//float Depth = TexDepth.SampleLevel( samplerPointClamp, Input.TextureUV, 0 ).r;
    float Depth = TexDepth.Load(uint3(Input.PositionH.xy, 0)).r;
    Depth = Depth * depthScaleFactors.x + depthScaleFactors.y;
    Depth = Depth * cameraNearFar.x + cameraNearFar.y;
    Depth = max(Depth, 0.0001);
	

    float FarPlane = 1.0 / Depth;
    FarPlane -= SkyboxFarPlane;

    FarPlane = saturate(FarPlane * 0.0001); // cb4_v18.x
	
    float factor = FarPlane * colorClouds.a;

	
    return float4(foo, factor);

	// line 77
    r0_z = 1; //cb4_v13.x - cb4_v12.x;
    r0_y = saturate(r0_y / r0_z);


	// line 79, perlin?
    float2 perlinUV = Time * 50 * float2(-0.002, 0.004);
    perlinUV = Input.TextureUV.xy * float2(1, 0.75) + perlinUV;

	// r0.z
    float perlin = TexPerlin.SampleLevel(samplerAnisoWrap, perlinUV, 0).x;

    float r0_w = 1.60 + 0.5; // cb4_15.x - cb4_14.x;
    perlin = saturate(perlin * r0_w - 0.5);

    r0_w = Input.TextureUV.x - 1;
    r0_w = saturate(1 * 2 + r0_w);

    perlin *= r0_w;

    r0_w = r0_y * perlin - 1.0;
    r0_y = r0_y * perlin - r0_w;

	// line 89
    factor = factor * r0_y + r0_w;

    r0_y = 1;
    r0_y = ViewDotLight * r0_y + 1;

    factor = saturate(r0_y * factor);
    factor = factor * 1;

    float4 finalColor = 0;
    finalColor.rgb = foo;
    finalColor.a = factor;

    return finalColor;
}
