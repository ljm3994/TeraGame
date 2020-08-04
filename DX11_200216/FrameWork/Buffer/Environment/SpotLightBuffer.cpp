#include "Framework.h"
#include "SpotLightBuffer.h"

SpotLightBuffer::SpotLightBuffer()
	: Constant(sizeof(data))
{
	data.LightCount = 0;
}

SpotLightBuffer::~SpotLightBuffer()
{
}

void SpotLightBuffer::Add(D3DXCOLOR Color, float Range, D3DXVECTOR3 Pos, D3DXVECTOR3 Dir, float Outer, float inner)
{
	if (data.LightCount < MAX_SPOTLIGHT)
	{
		data.lights[data.LightCount].Color = Color;
		data.lights[data.LightCount].Range = Range;
		data.lights[data.LightCount].Position = Pos;
		data.lights[data.LightCount].Direction = Dir;
		data.lights[data.LightCount].Outer = Outer;
		data.lights[data.LightCount].Inner = inner;
		data.LightCount++;
	}
}

SpotLightBuffer::SpotData* SpotLightBuffer::GetSpotData(int Index)
{
	if (Index < data.LightCount)
	{
		return &data.lights[data.LightCount];
	}

	return nullptr;
}

void SpotLightBuffer::MapData()
{
	D3D11_MAPPED_SUBRESOURCE subResource;

	Engine->GetContext()->Map(buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
	memcpy_s(subResource.pData, ArraySize, &data, ArraySize);
	Engine->GetContext()->Unmap(buffer.Get(), 0);
}
