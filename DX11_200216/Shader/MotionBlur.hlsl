#include "PostProcessHeader.hlsli"

Texture2D TexLightBuffer : register(t3);
Texture2D TexVelocityBuffer : register(t4);

float4 PS(in PostProcessOut Input) : SV_TARGET
{
    float2 Velocity = TexVelocityBuffer.Load(int3(Input.Position.xy, 0)).xy;

    float velocityScale = 16.667;
    velocityScale /= TimeDelta;
    velocityScale = min(velocityScale, 2000);
    
    Velocity *= velocityScale;

    float speed = length(Velocity / ViewPort.zw);
    uint nSamples = (uint) speed;
    nSamples = max(nSamples, 1);
    nSamples = min(nSamples, 64);

    float fSamples = (float) nSamples;
    
    float fSamplesMinusOne = fSamples - 1.0;

    float4 Diffuse = TexLightBuffer.Load(int3(Input.Position.xy, 0));

    float2 screenTexCoords = (Input.Position.xy + float2(0.5, 0.5)) * ViewPort.zw;
 
    for (uint i = 1; i < nSamples; ++i)
    {
        float2 offset = Velocity * ((float(i) / fSamplesMinusOne) - 0.5);
        offset += Input.Position.xy;

        int3 iPos = int3((int2) offset, 0);

        Diffuse += TexLightBuffer.Load(iPos);
		//Diffuse += TexLightBuffer.SampleLevel( samplerLinearClamp, screenTexCoords + offset, 0);
    }

    Diffuse /= fSamples;

    return Diffuse;
}