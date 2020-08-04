#include "Framework.h"
#include "DirectionLightBuffer.h"

DirectionLightBuffer::DirectionLightBuffer()
	: Constant(sizeof(data))
{
	data.LightDirection = D3DXVECTOR3(-1.0f, -1.0f, 1.0f);
	data.SpecExp = 8.0f;

	data.AmbientLight = D3DXCOLOR(0.1f, 0.1f, 0.1f, 0.1f);
}

DirectionLightBuffer::~DirectionLightBuffer()
{
}

void DirectionLightBuffer::SetDirection(D3DXVECTOR3 Direction)
{
	data.LightDirection = Direction;
}

void DirectionLightBuffer::SetSpecExp(float val)
{
	data.SpecExp = val;
}

void DirectionLightBuffer::SetAmbientLight(D3DXCOLOR Ambient)
{
	data.AmbientLight = Ambient;
}

void DirectionLightBuffer::MapData()
{
	D3D11_MAPPED_SUBRESOURCE subResource;

	Engine->GetContext()->Map(buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
	memcpy_s(subResource.pData, ArraySize, &data, ArraySize);
	Engine->GetContext()->Unmap(buffer.Get(), 0);
}
