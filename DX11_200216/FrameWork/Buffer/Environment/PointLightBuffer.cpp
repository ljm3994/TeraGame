#include "Framework.h"
#include "PointLightBuffer.h"

PointLightBuffer::PointLightBuffer()
	: Constant(sizeof(data))
{
	data.LightCount = 0;
}

PointLightBuffer::~PointLightBuffer()
{
}

void PointLightBuffer::Add(D3DXVECTOR3 Position, float range, D3DXCOLOR Color)
{
	if (data.LightCount < MAX_POINTLIGHT)
	{
		data.lights[data.LightCount].Position = Position;
		data.lights[data.LightCount].Range = range;
		data.lights[data.LightCount].Color = Color;
		data.LightCount++;
	}
}

PointLightBuffer::LightData* PointLightBuffer::GetLightData(int Index)
{
	if (Index < data.LightCount)
	{
		return &data.lights[Index];
	}

	return nullptr;
}

void PointLightBuffer::MapData()
{
	D3D11_MAPPED_SUBRESOURCE subResource;

	Engine->GetContext()->Map(buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
	memcpy_s(subResource.pData, ArraySize, &data, ArraySize);
	Engine->GetContext()->Unmap(buffer.Get(), 0);
}
