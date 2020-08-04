#include "MeshHeader.hlsli"

Texture2DArray CurrentBoneMap : register(t10);
Texture2DArray NextBoneMap : register(t11);
Texture2DArray cCurrentBoneMap : register(t12);
Texture2DArray cNextBoneMap : register(t13);

struct VertexUVNormalTangentModelInstance
{
    float4 position : POSITION;
    float2 uv : UV;
    float3 normal : NORMAL;
    float3 Tangent : TANGENT;
    float4 BlendIndex : BLENDINDICES;
    float4 BlendWeight : BLENDWEIGHT;
    
    matrix transform : INSTANCE;
    uint instanceID : SV_InstanceID;
};

struct PixelInput
{
    float4 position : SV_POSITION;
    float3 PositionWS : POSITIONWS;
    float4 PrevPos : POSITION;
    float DepthVS : DEPTHVS;
    float2 uv : UV;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
    float3 normal : NORMAL;
    float3 viewDir : VIEWDIR;
};

row_major matrix SkinWorld(float4 indices, float4 weights, uint instaceID)
{
    row_major matrix transform = 0;
    
    float4 c0, c1, c2, c3;
    c0 = CurrentBoneMap.Load(int4(indices.x * 4 + 0, (int) instaceID, 0, 0));
    c1 = CurrentBoneMap.Load(int4(indices.x * 4 + 1, (int) instaceID, 0, 0));
    c2 = CurrentBoneMap.Load(int4(indices.x * 4 + 2, (int) instaceID, 0, 0));
    c3 = CurrentBoneMap.Load(int4(indices.x * 4 + 3, (int) instaceID, 0, 0));
    
    transform += mul(weights.x, matrix(c0, c1, c2, c3));

    c0 = CurrentBoneMap.Load(int4(indices.y * 4 + 0, (int) instaceID, 0, 0));
    c1 = CurrentBoneMap.Load(int4(indices.y * 4 + 1, (int) instaceID, 0, 0));
    c2 = CurrentBoneMap.Load(int4(indices.y * 4 + 2, (int) instaceID, 0, 0));
    c3 = CurrentBoneMap.Load(int4(indices.y * 4 + 3, (int) instaceID, 0, 0));

    transform += mul(weights.y, matrix(c0, c1, c2, c3));
    
    c0 = CurrentBoneMap.Load(int4(indices.z * 4 + 0, (int) instaceID, 0, 0));
    c1 = CurrentBoneMap.Load(int4(indices.z * 4 + 1, (int) instaceID, 0, 0));
    c2 = CurrentBoneMap.Load(int4(indices.z * 4 + 2, (int) instaceID, 0, 0));
    c3 = CurrentBoneMap.Load(int4(indices.z * 4 + 3, (int) instaceID, 0, 0));
    
    transform += mul(weights.z, matrix(c0, c1, c2, c3));
    
    c0 = CurrentBoneMap.Load(int4(indices.w * 4 + 0, (int) instaceID, 0, 0));
    c1 = CurrentBoneMap.Load(int4(indices.w * 4 + 1, (int) instaceID, 0, 0));
    c2 = CurrentBoneMap.Load(int4(indices.w * 4 + 2, (int) instaceID, 0, 0));
    c3 = CurrentBoneMap.Load(int4(indices.w * 4 + 3, (int) instaceID, 0, 0));
    
    transform += mul(weights.w, matrix(c0, c1, c2, c3));
    
    row_major matrix transform2 = 0;

    float4 n0, n1, n2, n3;
    
    n0 = NextBoneMap.Load(int4(indices.x * 4 + 0, (int) instaceID, 0, 0));
    n1 = NextBoneMap.Load(int4(indices.x * 4 + 1, (int) instaceID, 0, 0));
    n2 = NextBoneMap.Load(int4(indices.x * 4 + 2, (int) instaceID, 0, 0));
    n3 = NextBoneMap.Load(int4(indices.x * 4 + 3, (int) instaceID, 0, 0));
    
    transform2 += mul(weights.x, matrix(n0, n1, n2, n3));
    
    n0 = NextBoneMap.Load(int4(indices.y * 4 + 0, (int) instaceID, 0, 0));
    n1 = NextBoneMap.Load(int4(indices.y * 4 + 1, (int) instaceID, 0, 0));
    n2 = NextBoneMap.Load(int4(indices.y * 4 + 2, (int) instaceID, 0, 0));
    n3 = NextBoneMap.Load(int4(indices.y * 4 + 3, (int) instaceID, 0, 0));
    
    transform2 += mul(weights.y, matrix(n0, n1, n2, n3));
 
    n0 = NextBoneMap.Load(int4(indices.z * 4 + 0, (int) instaceID, 0, 0));
    n1 = NextBoneMap.Load(int4(indices.z * 4 + 1, (int) instaceID, 0, 0));
    n2 = NextBoneMap.Load(int4(indices.z * 4 + 2, (int) instaceID, 0, 0));
    n3 = NextBoneMap.Load(int4(indices.z * 4 + 3, (int) instaceID, 0, 0));
    
    transform2 += mul(weights.z, matrix(n0, n1, n2, n3));

    n0 = NextBoneMap.Load(int4(indices.w * 4 + 0, (int) instaceID, 0, 0));
    n1 = NextBoneMap.Load(int4(indices.w * 4 + 1, (int) instaceID, 0, 0));
    n2 = NextBoneMap.Load(int4(indices.w * 4 + 2, (int) instaceID, 0, 0));
    n3 = NextBoneMap.Load(int4(indices.w * 4 + 3, (int) instaceID, 0, 0));
    
    transform2 += mul(weights.w, matrix(n0, n1, n2, n3));
    
    transform = lerp(transform, transform2, bones[instaceID].Bonesweight);

    if (bones[instaceID].isChanging != 0.0f)
    {
        row_major matrix transform3 = 0;

        c0 = cCurrentBoneMap.Load(int4(indices.x * 4 + 0, (int) instaceID, 0, 0));
        c1 = cCurrentBoneMap.Load(int4(indices.x * 4 + 1, (int) instaceID, 0, 0));
        c2 = cCurrentBoneMap.Load(int4(indices.x * 4 + 2, (int) instaceID, 0, 0));
        c3 = cCurrentBoneMap.Load(int4(indices.x * 4 + 3, (int) instaceID, 0, 0));
        
        transform3 += mul(weights.x, matrix(c0, c1, c2, c3));
        
        c0 = cCurrentBoneMap.Load(int4(indices.y * 4 + 0, (int) instaceID, 0, 0));
        c1 = cCurrentBoneMap.Load(int4(indices.y * 4 + 1, (int) instaceID, 0, 0));
        c2 = cCurrentBoneMap.Load(int4(indices.y * 4 + 2, (int) instaceID, 0, 0));
        c3 = cCurrentBoneMap.Load(int4(indices.y * 4 + 3, (int) instaceID, 0, 0));
        
        transform3 += mul(weights.y, matrix(c0, c1, c2, c3));
        
        c0 = cCurrentBoneMap.Load(int4(indices.z * 4 + 0, (int) instaceID, 0, 0));
        c1 = cCurrentBoneMap.Load(int4(indices.z * 4 + 1, (int) instaceID, 0, 0));
        c2 = cCurrentBoneMap.Load(int4(indices.z * 4 + 2, (int) instaceID, 0, 0));
        c3 = cCurrentBoneMap.Load(int4(indices.z * 4 + 3, (int) instaceID, 0, 0));
        
        transform3 += mul(weights.z, matrix(c0, c1, c2, c3));
        
        c0 = cCurrentBoneMap.Load(int4(indices.w * 4 + 0, (int) instaceID, 0, 0));
        c1 = cCurrentBoneMap.Load(int4(indices.w * 4 + 1, (int) instaceID, 0, 0));
        c2 = cCurrentBoneMap.Load(int4(indices.w * 4 + 2, (int) instaceID, 0, 0));
        c3 = cCurrentBoneMap.Load(int4(indices.w * 4 + 3, (int) instaceID, 0, 0));
        
        transform3 += mul(weights.w, matrix(c0, c1, c2, c3));
        
        row_major matrix transform4 = 0;

        n0 = cNextBoneMap.Load(int4(indices.x * 4 + 0, (int) instaceID, 0, 0));
        n1 = cNextBoneMap.Load(int4(indices.x * 4 + 1, (int) instaceID, 0, 0));
        n2 = cNextBoneMap.Load(int4(indices.x * 4 + 2, (int) instaceID, 0, 0));
        n3 = cNextBoneMap.Load(int4(indices.x * 4 + 3, (int) instaceID, 0, 0));
        
        transform4 += mul(weights.x, matrix(n0, n1, n2, n3));

        n0 = cNextBoneMap.Load(int4(indices.y * 4 + 0, (int) instaceID, 0, 0));
        n1 = cNextBoneMap.Load(int4(indices.y * 4 + 1, (int) instaceID, 0, 0));
        n2 = cNextBoneMap.Load(int4(indices.y * 4 + 2, (int) instaceID, 0, 0));
        n3 = cNextBoneMap.Load(int4(indices.y * 4 + 3, (int) instaceID, 0, 0));
        
        transform4 += mul(weights.y, matrix(n0, n1, n2, n3));
 
        n0 = cNextBoneMap.Load(int4(indices.z * 4 + 0, (int) instaceID, 0, 0));
        n1 = cNextBoneMap.Load(int4(indices.z * 4 + 1, (int) instaceID, 0, 0));
        n2 = cNextBoneMap.Load(int4(indices.z * 4 + 2, (int) instaceID, 0, 0));
        n3 = cNextBoneMap.Load(int4(indices.z * 4 + 3, (int) instaceID, 0, 0));
        
        transform4 += mul(weights.z, matrix(n0, n1, n2, n3));
 
        n0 = cNextBoneMap.Load(int4(indices.w * 4 + 0, (int) instaceID, 0, 0));
        n1 = cNextBoneMap.Load(int4(indices.w * 4 + 1, (int) instaceID, 0, 0));
        n2 = cNextBoneMap.Load(int4(indices.w * 4 + 2, (int) instaceID, 0, 0));
        n3 = cNextBoneMap.Load(int4(indices.w * 4 + 3, (int) instaceID, 0, 0));
        
        transform4 += mul(weights.w, matrix(n0, n1, n2, n3));
        
        transform3 = lerp(transform3, transform4, bones[instaceID].Bonesweight);

        transform = lerp(transform, transform3, ChangeBones[instaceID].ChangingWeight);
    }
    return transform;
}

PixelInput VS(VertexUVNormalTangentModelInstance input)
{
    PixelInput output = (PixelInput) 0;
    
    if (isActive[input.instanceID])
    {
        row_major matrix boneWorld = input.transform;

        if (bones[input.instanceID].HasAnimation != 0.0f)
        {
            boneWorld = mul(SkinWorld(input.BlendIndex, input.BlendWeight, input.instanceID), input.transform);
        }

        output.position = mul(input.position, boneWorld);
    
        output.PositionWS = output.position;

        output.DepthVS = mul(output.position, view).z;
        
        float3 camPos = invView._41_42_43;
        output.viewDir = normalize(output.position.xyz - camPos);
    
        output.PrevPos = mul(output.position, prevView);
        output.PrevPos = mul(output.PrevPos, prevProj);
        
        output.position = mul(output.position, view);
        output.position = mul(output.position, projection);
        output.uv = input.uv;
    
        output.normal = mul(input.normal, (float3x3) boneWorld);
        output.tangent = mul(input.Tangent, (float3x3) boneWorld);
        output.binormal = cross(output.normal, output.tangent);
    }
    return output;
}

PS_OUT PS(in PixelInput input)
{
    float3 vtxNormal = normalize(input.normal);
    float3 positionWS = input.PositionWS;
    float3 viewWS = normalize(CameraPos - positionWS);
    float3 normalWS = vtxNormal;
    
    float3 normalTS = float3(0, 0, 1);
    float3 tangentWS = normalize(input.tangent);
    float3 bitangentWS = normalize(input.binormal);
    float3x3 tangentToWorld = float3x3(tangentWS, bitangentWS, normalWS);

    normalTS.xy = normalMap.Sample(AnisoSampler, input.uv).xy * 2.0f - 1.0f;
    normalTS.z = sqrt(1.0f - saturate(normalTS.x * normalTS.x + normalTS.y * normalTS.y));
    normalTS = lerp(float3(0, 0, 1), normalTS, NormalMapIntensity);
    normalWS = normalize(mul(normalTS, tangentToWorld));

    float3 albedoMap = 1.0f;
    
    albedoMap = diffuseMap.Sample(AnisoSampler, input.uv).xyz;
    
    float3 diffuseAlbedo = albedoMap.xyz;
    diffuseAlbedo *= DiffuseIntensity;
    diffuseAlbedo *= (1.0f - SpecularIntensity);
    
    float shadowVisibility = EnableShadows ? ShadowVisibility(positionWS, input.DepthVS) : 1.0f;
    float3 lighting = 0.0f;
    
    if (EnableDirectLighting)
        lighting += CalcLighting(normalWS, LightDirection, LightColor, diffuseAlbedo, SpecularIntensity,
                                 Roughness, positionWS) * shadowVisibility;
    
    if (EnableAmbientLighting)
    {
        float3 indirectDiffuse = EvalCosine(normalWS, EnvironmentSH);

        lighting += indirectDiffuse * diffuseAlbedo;

        float3 reflectWS = reflect(-viewWS, normalWS);
        float3 vtxReflectWS = reflect(-viewWS, vtxNormal);

        uint width, height, numMips;
        SpecularCubemap.GetDimensions(0, width, height, numMips);

        const float SqrtRoughness = sqrt(Roughness);

        float mipLevel = saturate(SqrtRoughness - 0.01f) * (numMips - 1.0f);

        float gradientMipLevel = SpecularCubemap.CalculateLevelOfDetail(LinearSampler, vtxReflectWS);
        if (UseGradientMipLevel)
            mipLevel = max(mipLevel, gradientMipLevel);

        float viewAngle = saturate(dot(viewWS, normalWS));
        float2 AB = SpecularCubemapLookup.SampleLevel(LinearSampler, float2(viewAngle, SqrtRoughness), 0.0f);
        float fresnel = SpecularIntensity * AB.x + AB.y;
        fresnel *= saturate(SpecularIntensity * 100.0f);

        lighting += SpecularCubemap.SampleLevel(LinearSampler, reflectWS, mipLevel) * fresnel;
    }
    
    PS_OUT output;
    output.Color = float4(lighting, 1.0f);
    
    output.Color.xyz *= exp2(ExposureScale);
    
    float2 prevPositionSS = (input.PrevPos.xy / input.PrevPos.z) * float2(0.5f, -0.5f) + 0.5f;
    prevPositionSS *= RTSize;
    output.Velocity = input.position.xy - prevPositionSS;
    output.Velocity -= JitterOffset;
    output.Velocity /= RTSize;
    
    return output;
}