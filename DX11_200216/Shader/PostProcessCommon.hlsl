#include "PostProcessHeader.hlsli"

float2 VignetteSpace(float2 Pos)
{
    float4 ViewportSize = ViewPort;

    float Scale = sqrt(2.0) / sqrt(1.0 + (ViewportSize.y * ViewportSize.z) * (ViewportSize.y * ViewportSize.z));
    return Pos * float2(1.0, ViewportSize.y * ViewportSize.z) * Scale;
}

PostProcessOut VS(in uint id : SV_VertexID)
{
    PostProcessOut Output;

    //Output.TextureUV = float2((id << 1) & 2, id & 2);
    //Output.Position = float4(Output.TextureUV * float2(2, -2) + float2(-1, 1), 0.f, 1.f);
    Output.Position.x = float(id / 2) * 4.0f - 1.0f;
    Output.Position.y = float(id % 2) * 4.0f - 1.0f;
    Output.Position.z = 0.0;
    Output.Position.w = 1.0;
    
    Output.TextureUV.x = (float) (id / 2) * 2.0;
    Output.TextureUV.y = 1.0 - (float) (id % 2) * 2.0;
    Output.Params = Output.Position.xy * 0.5 + 0.5;

    return Output;
}

float4 PS(PostProcessOut Input) : SV_Target0
{
    float4 color = InputTexture0.Sample(samplerLinearClamp, Input.TextureUV);
    return color;
}