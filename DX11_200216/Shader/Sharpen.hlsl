#include "PostProcessHeader.hlsli"

Texture2D TexColorBuffer : register(t0);
Texture2D TexDepthBuffer : register(t1);

cbuffer cbSharpen : register(b5)
{
    float4 sharpenParams1;
    float4 sharpenParams2;
}

float4 PS(in PostProcessOut Input) : SV_TARGET
{
    float fDepth = TexDepthBuffer.Load(int3(Input.Position.xy, 0)).x;
    fDepth = fDepth * depthScaleFactors.x + depthScaleFactors.y;

    float fScaledDepthNearFar = fDepth * cameraNearFar.x + cameraNearFar.y;
    fScaledDepthNearFar = max(fScaledDepthNearFar, 1e-4);

    float fScaledDepth = 1.0 / fScaledDepthNearFar;

    float fSharpenDepthRange = saturate(fScaledDepth * sharpenParams1.z - sharpenParams1.w);
	
    float fBias = sharpenParams1.y - sharpenParams1.x;

    float fWhatever = fSharpenDepthRange * fBias + sharpenParams1.x;
    fWhatever += 1.0;

    float fSkyboxTest = (fDepth >= 1.0);
    float fSkyboxBinaryFilter = (fSkyboxTest) ? 0 : 1;

    float fFoo = fWhatever * fSkyboxBinaryFilter;

    float2 uvStart = trunc(Input.Position.xy);
    uvStart += float2(0.5, 0.5);
    uvStart /= ViewPort.xy;

    float3 finalColor = 0;
    finalColor = TexColorBuffer.SampleLevel(samplerLinearClamp, uvStart, 0).rgb;
	[branch]
    if (fFoo > 0)
    {
        float2 uvOffset = float2(0.5, 0.5) / ViewPort.xy;
        float3 color1 = 0;
		
		// -0,5, -0.5
        float2 uvOffsetOther = uvStart - uvOffset;
        color1 += TexColorBuffer.SampleLevel(samplerLinearClamp, uvOffsetOther, 0).rgb;

        float2 uvOffsetInv = -uvOffset;
        float2 uvOffsetOther2 = uvStart + float2(uvOffsetInv.x, -uvOffsetInv.y);
        color1 += TexColorBuffer.SampleLevel(samplerLinearClamp, uvOffsetOther2, 0).rgb;

        uvOffsetOther2 = uvStart + float2(-uvOffsetInv.x, uvOffsetInv.y);
        color1 += TexColorBuffer.SampleLevel(samplerLinearClamp, uvOffsetOther2, 0).rgb;

		// +0.5, -0.5
        uvOffsetOther = uvStart + uvOffset;
        color1 += TexColorBuffer.SampleLevel(samplerLinearClamp, uvOffsetOther, 0).rgb;

        float3 averageColor = color1 / 4;

        float3 diffColor = -color1 / 4.0 + finalColor;

        float fDiffColorMaxComponent = max(abs(diffColor.x), max(abs(diffColor.y), abs(diffColor.z)));

        float fDiffColorMaxComponentScaled = saturate(fDiffColorMaxComponent * sharpenParams2.x + sharpenParams2.y);

        float fFoobar = fWhatever * fSkyboxBinaryFilter - 1.0;
        fFoobar = fDiffColorMaxComponentScaled * fFoobar + 1.0;
	
        float fLuminanceFirstColor = dot(LUMINANCE_RGB, finalColor);
        float fLuminanceAvgColor = dot(LUMINANCE_RGB, averageColor);

        float3 fColorBalanced = finalColor / max(fLuminanceFirstColor, 1e-4);
		
        float fLumDiff = fLuminanceFirstColor - fLuminanceAvgColor;
        float fPixelLuminance = fFoobar * fLumDiff + fLuminanceAvgColor;
        fPixelLuminance = max(fPixelLuminance, 0.0);

        finalColor = fColorBalanced * fPixelLuminance;
    }

    return float4(finalColor, 1);
}