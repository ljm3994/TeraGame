#include "Framework.h"
#include "InstanceBuffer.h"

InstanceBuffer::InstanceBuffer()
	: Constant(sizeof(Data))
{
	for (UINT i = 0; i < MAX_INSTANCECOUNT; i++)
	{
		data.isActive[i] = true;
	}
}

InstanceBuffer::~InstanceBuffer()
{
}

void InstanceBuffer::SetIsActive(UINT instanceID, bool val)
{
	data.isActive[instanceID] = val;
}

bool InstanceBuffer::GetIsActive(UINT instanceID)
{
	return data.isActive[instanceID];
}

void InstanceBuffer::MapData()
{
	D3D11_MAPPED_SUBRESOURCE subResource;

	Engine->GetContext()->Map(buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
	memcpy_s(subResource.pData, ArraySize, &data, ArraySize);
	Engine->GetContext()->Unmap(buffer.Get(), 0);
}
