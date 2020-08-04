#include "Header.hlsli"

struct VertexUVNormalTangentModelInstance
{
    float4 position : POSITION;
    float2 uv : UV;
    float3 normal : NORMAL;
    float3 Tangent : TANGENT;
 
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
};

PixelInput VS(VertexUVNormalTangentModelInstance input)
{   
    PixelInput output = (PixelInput) 0;
    
    if (isActive[input.instanceID])
    {
        output.position = mul(input.position, input.transform);
    
        float3 camPos = invView._41_42_43;
        output.viewDir = normalize(output.position.xyz - camPos);
    
        output.PrevPos = mul(output.position, prevView);
        output.PrevPos = mul(output.PrevPos, prevProj);
        
        output.position = mul(output.position, view);
        output.position = mul(output.position, projection);
        output.uv = input.uv;
    
        output.normal = mul(input.normal, (float3x3) input.transform);
        output.tangent = mul(input.Tangent, (float3x3) input.transform);
        output.binormal = cross(output.normal, output.tangent);
    }
    
    return output;
}

PS_OUTPUT PS(PixelInput input)
{
    PS_OUTPUT output;
    
    float3 light = normalize(lightDirection);
    
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