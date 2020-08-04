#include "Framework.h"
#include "TrailBoneBuffer.h"

TrailBoneBuffer::TrailBoneBuffer()
	: Constant(sizeof(Data))
{
	for (UINT i = 0; i < TRAIL_BONE_MAX; i++)
	{
		D3DXMatrixIdentity(&data.World[i]);
	}

	data.Count = 1;
}

void TrailBoneBuffer::MapData()
{
	D3D11_MAPPED_SUBRESOURCE subResource;

	Engine->GetContext()->Map(buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
	memcpy_s(subResource.pData, ArraySize, &data, ArraySize);
	Engine->GetContext()->Unmap(buffer.Get(), 0);
}
