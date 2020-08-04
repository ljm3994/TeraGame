#include "PostProcessHeader.hlsli"

Texture2D TexColorBuffer : register(t3);

float3 LensFlaresThreshold(in PostProcessOut Input) : SV_TARGET
{
	// Flip texcoords
    float2 LensFlaresTexcoords = -Input.TextureUV + float2(1.0, 1.0);

    float3 color = TexColorBuffer.SampleLevel(samplerLinearClamp, LensFlaresTexcoords, 0).rgb;
    color -= 15;
	
    color = max(color, 0);
    color *= 0.25;

    return color;
}

float3 LensFlareTextureDistorted(in Texture2D Tex, in float2 TextureUV, in float2 Direction, in float3 Distortion)
{
    return float3(Tex.Sample(samplerLinearClamp, TextureUV + Direction * Distortion.r).r,
				   Tex.Sample(samplerLinearClamp, TextureUV + Direction * Distortion.g).g,
				   Tex.Sample(samplerLinearClamp, TextureUV + Direction * Distortion.b).b
				   );

}

float3 LensFlaresGhosts(in PostProcessOut Input) : SV_TARGET
{
    int g_nLensFlareNumGhosts = 8;
    float g_fLensFlareDistortion = 10;
    float g_fLensFlareDispersal = 0.37;
    float g_fLensFlareHaloWidth = 0.45;

    static const float RcpOfHalfVector2 = 1.0 / length(float2(0.5, 0.5));


    float2 uv = Input.TextureUV; // + float2(1.0, 1.0);
    float2 texelSize = ViewPort.zw * 0.25;

    const float3 vDistortion = float3(-texelSize.x * g_fLensFlareDistortion, 0, +texelSize.x * g_fLensFlareDistortion);

	// Ghost vector to image centre.
    float2 vGhost = float2(0.5, 0.5) - uv;
    vGhost *= g_fLensFlareDispersal;

    float2 haloVec = normalize(vGhost) * g_fLensFlareHaloWidth;


	// Sample ghosts
    float4 result = 0.0;
	[unroll]
    for (int i = 0; i < g_nLensFlareNumGhosts; ++i)
    {
        float2 offset = frac(uv + vGhost * (float) i);

        float weight = length(float2(0.5, 0.5) - offset) * RcpOfHalfVector2;

        weight = pow(1.0 - weight, 10);

        result.rgb += LensFlareTextureDistorted(TexColorBuffer, offset, normalize(vGhost), vDistortion) * weight;
    }

    float texLensColor = length(float2(0.5, 0.5) - uv) * RcpOfHalfVector2;
	//result *= TexLC.SampleLevel(samplerPointClamp, float2(texLensColor, 0), 0.0);

	// Sample halo	

    float halo_weight = length(float2(0.5, 0.5) - frac(uv + haloVec)) * RcpOfHalfVector2;
    halo_weight = pow(1.0 - halo_weight, 15);

    result.rgb += LensFlareTextureDistorted(TexColorBuffer, frac(uv + haloVec), normalize(vGhost), vDistortion) * halo_weight;


	// Lens dirt (to further phase)
	//result.rgb *= pow(Tex1.Sample(samplerPointClamp, uv).rgb, 2.2).rgb;

    return max(0, result);
}