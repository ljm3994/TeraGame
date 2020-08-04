#include "Framework.h"
#include "ViewDataBuffer.h"

ViewDataBuffer::ViewDataBuffer()
	: Constant(sizeof(data))
{
	float width = Engine->GetClientWidth();
	float height = Engine->GetClientHeight();

	data.ViewData.x = width;
	data.ViewData.y = height;
	data.ViewData.z = 1.0f / width;
	data.ViewData.w = 1.0f / height;
}

ViewDataBuffer::~ViewDataBuffer()
{
}

void ViewDataBuffer::SetViewData(float Width, float Height, float z, float w)
{
	data.ViewData.x = Width;
	data.ViewData.y = Height;
	data.ViewData.z = z;
	data.ViewData.w = w;
}

void ViewDataBuffer::MapData()
{
	D3D11_MAPPED_SUBRESOURCE subResource;

	Engine->GetContext()->Map(buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
	memcpy_s(subResource.pData, ArraySize, &data, ArraySize);
	Engine->GetContext()->Unmap(buffer.Get(), 0);
}
