#include "Framework.h"
#include "TrailColorBuffer.h"

TrailColorBuffer::TrailColorBuffer()
	: Constant(sizeof(data))
{
	data.Color = D3DXCOLOR(1, 0.1f, 0.7f, 0.7f);
}

void TrailColorBuffer::MapData()
{
	D3D11_MAPPED_SUBRESOURCE subResource;

	Engine->GetContext()->Map(buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
	memcpy_s(subResource.pData, ArraySize, &data, ArraySize);
	Engine->GetContext()->Unmap(buffer.Get(), 0);
}

void TrailColorBuffer::SetColor(D3DXCOLOR color)
{
	data.Color = color;
}
