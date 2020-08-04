#include "Framework.h"
#include "WorldBuffer.h"

WorldBuffer::WorldBuffer()
	: Constant(sizeof(W))
{
}

WorldBuffer::~WorldBuffer()
{
}

void WorldBuffer::SetWorld(D3DXMATRIX value)
{
	Original = value;
	D3DXMatrixTranspose(&Data.World, &value);
}

D3DXMATRIX WorldBuffer::GetWorld()
{
	return Original;
}

void WorldBuffer::MapData()
{
	D3D11_MAPPED_SUBRESOURCE subResource;

	Engine->GetContext()->Map(buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
	memcpy_s(subResource.pData, ArraySize, &Data, ArraySize);
	Engine->GetContext()->Unmap(buffer.Get(), 0);
}
