cbuffer VP : register(b1)
{
    matrix view;
    matrix projection;
    matrix inView;
}

cbuffer W : register(b0)
{
    matrix world;
}


struct VertexInput
{
    float4 position : POSITION;
    float4 Color : COLOR;
};

struct PixelInput
{
    float4 position : SV_POSITION;
    float4 Color : COLOR;
};

PixelInput VS(VertexInput input)
{
    PixelInput output;
    output.position = mul(input.position, world);
    output.position = mul(output.position, view);
    output.position = mul(output.position, projection);
    
    output.Color = input.Color;
    
    return output;
}

float4 PS(PixelInput input) : SV_TARGET0
{
    return input.Color;
}