#include "PostProcessHeader.hlsli"

float CalcGaussianWeight(int sampleDist, float sigma)
{
    float twoSigma2 = 2 * sigma * sigma;
    float g = 1.0f / sqrt(PI * twoSigma2);
    return (g * exp(-(sampleDist * sampleDist) / (twoSigma2)));
}

float4 Blur(in PostProcessOut input, float2 texScale, float sigma)
{
    float2 TextureSize = ViewPort.xy * 0.25;

    float4 color = 0;
    for (int i = -4; i < 4; i++)
    {
        float weight = CalcGaussianWeight(i, sigma);
        float2 texCoord = input.TextureUV;
        texCoord += (i / TextureSize) * texScale;
        float4 sample = InputTexture0.Sample(samplerPointClamp, texCoord);
        color += sample * weight;
    }
	
    return color;
}

float4 LensFlaresBlurV(PostProcessOut Input) : SV_Target
{
    return Blur(Input, float2(0, 1), 1.5);
}

float4 LensFlaresBlurH(PostProcessOut Input) : SV_Target
{
    return Blur(Input, float2(1, 0), 1.5);
}