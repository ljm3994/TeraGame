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


struct VS_OUTPUT
{
    float4 PositionH : SV_POSITION;
    float4 Position0 : POSITION0;
    float4 PositionPrev : POSITION1;
};

VS_OUTPUT VS(VertexUVNormalTangentModelInstance input)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    
    float4 pos = mul(input.position, world);
    
    output.PositionH = mul(pos, view);
    output.PositionH = mul(output.PositionH, projection);
    
    output.Position0 = output.PositionH;
    output.PositionPrev = mul(pos, prevView);
    output.PositionPrev = mul(output.PositionPrev, prevProj);

    return output;
}

float2 PS(VS_OUTPUT Input) : SV_Target0
{
    float2 currentPos = Input.Position0.xy / Input.Position0.w;
    currentPos = currentPos * float2(0.5, 0.5) + float2(0.5, 0.5);

    float2 previousPos = Input.PositionPrev.xy / Input.PositionPrev.w;
    previousPos = previousPos * float2(0.5, 0.5) + float2(0.5, 0.5);

    float2 velocity = currentPos - previousPos;

    return velocity;
}