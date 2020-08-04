#include "Framework.h"
#include "PostProcesPS.h"

PostProcesPS::PostProcesPS()
    : Constant(sizeof(Data))
{
}

PostProcesPS::~PostProcesPS()
{
}

void PostProcesPS::MapData()
{
	D3D11_MAPPED_SUBRESOURCE subResource;

	Engine->GetContext()->Map(buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
	memcpy_s(subResource.pData, ArraySize, &data, ArraySize);
	Engine->GetContext()->Unmap(buffer.Get(), 0);
}
