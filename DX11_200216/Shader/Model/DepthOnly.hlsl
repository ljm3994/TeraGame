#include "../Header.hlsli"

struct VSInput
{
    float4 PositionOS : POSITION;
};

struct VSOutput
{
    float4 PositionCS : SV_Position;
};

VSOutput VS(in VSInput input)
{
    VSOutput output;

    output.PositionCS = mul(input.PositionOS, world);
    output.PositionCS = mul(output.PositionCS, view);
    output.PositionCS = mul(output.PositionCS, projection);
    
    return output;
}