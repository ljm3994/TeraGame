#include "PostProcessHeader.hlsli"

Texture2D TexColorBuffer : register(t3);
Texture2D TexNoise : register(t4);
Texture2D TexVignette : register(t5);
Texture2D TexLensFlares : register(t6);
Texture2D TexLensDirt : register(t7);

static const float2 drunkOffsets[8] =
{
    float2(1.0, 0.0),
	float2(-1.0, 0.0),
	float2(0.707, 0.707),
	float2(-0.707, -0.707),
	float2(0.0, 1.0),
	float2(0.0, -1.0),
	float2(-0.707, 0.707),
	float2(0.707, -0.707)
};

float4 vodka(float2 uv)
{
    float4 cb0_v0 = float4(119, 49, 86119.70, 0.99);
    float4 cb0_v1 = ViewPort;
    float4 cb3_v0 = float4(1, 1, ViewPort.zw);
    float4 cb3_v1;
    sincos(Time * 200, cb3_v1.x, cb3_v1.y);
	
	//return float4(cb3_v1.x, cb3_v1.y, 0, 0);

	// Writing from scratch.
    float fAmount = cb3_v0.y * (-0.1) + 1.0;

    float2 offsetsSmall = cb3_v1.xy * float2(0.05, 0.05);

	// distance from center, r1.z
    float2 centerToTexel = uv - float2(0.5, 0.5);
    float distanceFromCenter = length(centerToTexel);

    float fScaledAmount = min(distanceFromCenter * 10, 1.0); // not higher than 1.0
    fScaledAmount *= cb3_v0.y;
	
	// next, we scale offsets by distance from center. Makes perfect sense :)
    float2 offsets = offsetsSmall * distanceFromCenter;
    offsets = centerToTexel * fAmount - offsets;
    offsets += float2(0.5, 0.5);

	// line 10
    float2 r3_xy = fAmount * centerToTexel;
    r3_xy = offsetsSmall * distanceFromCenter + r3_xy;
    r3_xy += float2(0.5, 0.5);
	

	// line 14
    float fScaledAmount2 = fScaledAmount * cb3_v0.x;
    fScaledAmount2 *= 5.0;

	// r4.xy.
    float2 xOffsets = fScaledAmount2 * ViewPort.zw;

    float4 total = 0;
    float4 total2 = 0;
    int i = 0;

	[unroll]
    for (i = 0; i < 8; i++)
    {
        total += TexColorBuffer.Sample(samplerLinearClamp, xOffsets * drunkOffsets[i] + offsets.xy);
    }
    total /= 16.0;

	[unroll]
    for (i = 0; i < 8; i++)
    {
        total2 += TexColorBuffer.Sample(samplerLinearClamp, xOffsets * drunkOffsets[i] + r3_xy.xy);
    }
    total2 /= 16.0;

	

    float4 final = total + total2;
	
	// some other stuff.

	// 57
    float fAmountY = cb3_v0.y * 8.0;

	// 58
    float2 offsets2 = fAmountY * ViewPort.zw; // r0.xy

	// 59
    float fExtraAmount = cb3_v1.y * 0.02 + 1.0;

	// 60
    float2 r1_zw = fExtraAmount * centerToTexel;

	// 61
    centerToTexel = centerToTexel * fExtraAmount + float2(0.5, 0.5);

	// 62
    r3_xy = r1_zw * offsets2 + centerToTexel;

    float2 r0_xy = offsets2 * r1_zw;
    r0_xy = r0_xy * 2.0 + centerToTexel;


    float4 a = TexColorBuffer.Sample(samplerLinearClamp, centerToTexel);
    float4 b = TexColorBuffer.Sample(samplerLinearClamp, r0_xy);
    float4 c = TexColorBuffer.Sample(samplerLinearClamp, r3_xy);

    float4 abc = a + b + c;
    abc /= 3.0;
	

    float4 final_r0 = fScaledAmount * (final - abc);
    float4 finalColor = cb3_v0.y * final_r0 + abc;
	
    return finalColor;
}

float TheWitcher3_2015_Mask(in float2 uv)
{
    float distanceFromCenter = length(uv - float2(0.5, 0.5));

    float x = distanceFromCenter * 2.0 - 0.55;
    x = saturate(x * 1.219512);
	
    float x2 = x * x;
    float x3 = x2 * x;
    float x4 = x2 * x2;

    float outX = dot(float4(x4, x3, x2, x), float4(-0.10, -0.105, 1.12, 0.09));
    outX = min(outX, 0.94);

    return outX;
}

float3 Vignette_TW3(in float3 gammaColor, in float3 vignetteColor, in float3 vignetteWeights, in float vignetteOpacity, in Texture2D texVignette, in float2 texUV)
{
	// For coloring vignette
    float3 vignetteColorGammaSpace = -gammaColor + vignetteColor;

	// Calculate vignette amount based on color in *LINEAR* color space and vignette weights.
    float vignetteWeight = dot(GammaToLinear(gammaColor), vignetteWeights);

	// We need to keep vignette weight in [0-1] range
    vignetteWeight = saturate(1.0 - vignetteWeight);

	// Multiply by opacity
    vignetteWeight *= vignetteOpacity;

	// Obtain vignette mask (here is texture; you can also calculate your custom mask here)
    float sampledVignetteMask = texVignette.Sample(samplerLinearClamp, texUV).x;
	
	// Final (inversed) vignette mask
    float finalInvVignetteMask = saturate(vignetteWeight * sampledVignetteMask);

	// final composite in gamma space
    float3 Color = vignetteColorGammaSpace * finalInvVignetteMask + gammaColor.rgb;
	
	// * uncomment to debug vignette mask:
#if (SHOW_VIGNETTE == 1)
	return 1.0 - finalInvVignetteMask;
#endif

	// Return final color
    return Color;
}

float4 PS(PostProcessOut Input) : SV_Target
{
    float2 TextureUV = Input.TextureUV;
    float4 ColorBuffer = TexColorBuffer.Sample(samplerLinearClamp, TextureUV);

    float2 TextureUV2 = trunc(Input.Position.xy);

    TextureUV2 /= ViewPort.xy;
    float4 ColorBuffer2 = TexColorBuffer.Sample(samplerLinearClamp, TextureUV2);
    float fTest = dot(ColorBuffer2, ColorBuffer2) / dot(ColorBuffer2, ColorBuffer);
    fTest = pow(fTest, 2);

    ChromaticAberration(TexColorBuffer, TextureUV, 2, ColorBuffer.rgb);

    float3 finalFilter = float3(1.0, 1.0, 1.0);

    float3 gammaColor = LinearToGamma(ColorBuffer.rgb);

	// apply final filter
    gammaColor *= finalFilter;

	// *The Witcher 3 vignette*
    float3 Color = Vignette_TW3(gammaColor, TW3VignetteColor.rgb, TW3VignetteWihght.rgb, TW3VignetteOpacity, TexVignette, TextureUV);
	//Color = TheWitcher3_2015_Mask(TextureUV);
    float lumaFXAA = dot(Color, float3(0.299, 0.587, 0.114)); // compute luma

	/* Film Grain */
    float2 noiseScale = float2(24.0, 13.5);
    float2 noiseOffset = 25 * float2(Time, Time * 0.33333);

    float3 noise = TexNoise.SampleLevel(samplerPointWrap, Input.TextureUV * noiseScale + noiseOffset, 0).rrr;
	//Color = lerp( Color, noise, 0.05 );
    Color += noise * 0.01;


    return float4(Color.rgb, lumaFXAA);
}