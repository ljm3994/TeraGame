#include "Header.hlsli"

#define TRAIL_BONE_MAX 200

cbuffer TrailBone : register(b10)
{
    matrix TrailWorld[TRAIL_BONE_MAX];
    int TrailCount;
}

cbuffer TrailColor : register(b10)
{
    float4 TrailColor;
}

struct PixelInput
{
    float4 Position : SV_Position;
    float2 uv : UV;
};

PixelInput VS(VertexUV Input)
{
    PixelInput output;
    
    float rate = 1.0f / (float) TrailCount;
    int index = (int) (Input.uv.x / rate);
    
    index = min(index, TrailCount - 1);
    matrix finalworld = TrailWorld[index];
    
    output.Position = mul(Input.position, finalworld);
    output.Position = mul(output.Position, view);
    output.Position = mul(output.Position, projection);

    output.uv = Input.uv;
    
    return output;
}

float4 PS(PixelInput Input) : SV_Target0
{
    float4 color;

    color = diffuseMap.Sample(diffuseSamp, Input.uv);
    color.a = 1.0f - Input.uv.x;
    
    return color;
}