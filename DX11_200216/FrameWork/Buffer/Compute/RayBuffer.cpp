#include "Framework.h"
#include "RayBuffer.h"

RayBuffer::RayBuffer()
	: Constant(sizeof(Data))
{
	data.position = D3DXVECTOR3(0, 0, 0);
	data.size = 0;
	data.direction = D3DXVECTOR3(0, 0, 0);
}

RayBuffer::~RayBuffer()
{
}

void RayBuffer::SetRay(const Ray& ray)
{
	data.position = ray.position;
	data.direction = ray.direction;
}

void RayBuffer::SetSize(UINT Size)
{
	data.size = Size;
}

void RayBuffer::MapData()
{
	D3D11_MAPPED_SUBRESOURCE subResource;

	Engine->GetContext()->Map(buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
	memcpy_s(subResource.pData, ArraySize, &data, ArraySize);
	Engine->GetContext()->Unmap(buffer.Get(), 0);
}
