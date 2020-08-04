#include "PostProcessHeader.hlsli"

Texture2D TexInput : register(t0);

float PS(PostProcessOut Input) : SV_TARGET
{
    float3 color = TexInput.SampleLevel(samplerAnisoWrap, Input.TextureUV, 0).rgb;

    float luminance = dot(color, LUMINANCE_RGB);
    luminance = clamp(luminance, 1e-5, 100000.0);
    luminance = log(luminance);

    return luminance;
}