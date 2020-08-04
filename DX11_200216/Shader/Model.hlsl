#include "Header.hlsli"

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
    float4 PrevPos : POSITION;
    float2 uv : UV;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
    float3 normal : NORMAL;
    float3 viewDir : VIEWDIR;
    uint InstanceID : InstanceID;
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
    
    output.InstanceID = input.instanceID;
    if (isActive[input.instanceID] == 1)
    {
        row_major matrix boneWorld = input.transform;
        output.position = input.position;

        if (bones[input.instanceID].HasAnimation != 0.0f)
        {
            boneWorld = mul(SkinWorld(input.BlendIndex, input.BlendWeight, input.instanceID), input.transform);
        }

        output.position = mul(output.position, boneWorld);
    
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

PS_OUTPUT PS(PixelInput input)
{    
    PS_OUTPUT output;
    
    float3 T = normalize(input.tangent);
    float3 B = normalize(input.binormal);
    float3 N = normalize(input.normal);
    
    float3 normal = N;
    
    float3x3 TBN = float3x3(T, B, N);
    
    if(isNormalMap == 1)
    {
        float4 normalMapping = normalMap.Sample(normalSamp, input.uv);
        normal = normalMapping.xyz * 2.0f - 1.0f;
        normal = normalize(mul(normal, TBN));
    }
    //normal = normalize(mul(normal, TBN));
    CompressUnsignedNormalToNormalsBuffer(texNormalsFit, normal);
    
    float4 albedo = diffuseMap.Sample(diffuseSamp, input.uv);
    
    float4 specularMapping = float4(1.0f, 1.0f, 1.0f, 1.0f);
    
    if(isSpecMap == 1)
    {
        specularMapping = specularMap.Sample(specularSamp, input.uv);
    }
    
    float2 currentPos = input.position.xy / input.position.w;
    currentPos = currentPos * float2(0.5, 0.5) + float2(0.5, 0.5);
    
    float2 previousPos = input.PrevPos.xy / input.PrevPos.w;
    previousPos = previousPos * float2(0.5, 0.5) + float2(0.5, 0.5);
    
    float2 velocity = currentPos - previousPos;
    
    output.RT0 = float4(albedo.rgb, 1.0f);
    output.RT1 = float4(normal_encode_xyz(normal), 1.0f);
    output.RT2 = specularMapping;
    output.RT3 = velocity;
     
    return output;
}