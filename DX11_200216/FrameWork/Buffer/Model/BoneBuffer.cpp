#include "Framework.h"
#include "BoneBuffer.h"

BoneBuffer::BoneBuffer()
	: Constant(sizeof(Data))
{
}

void BoneBuffer::MapData()
{
	D3D11_MAPPED_SUBRESOURCE subResource;

	Engine->GetContext()->Map(buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
	memcpy_s(subResource.pData, ArraySize, &data, ArraySize);
	Engine->GetContext()->Unmap(buffer.Get(), 0);
}