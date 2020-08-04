#include "Framework.h"
#include "ViewProjectionBuffer.h"

ViewProjection::ViewProjection() : Constant(sizeof(VP))
{
	D3DXMatrixIdentity(&Data.View);
	D3DXMatrixIdentity(&Data.Projection);
	D3DXMatrixIdentity(&Data.InvView);
}

ViewProjection::~ViewProjection()
{
}

void ViewProjection::SetViewData(D3DXMATRIX View)
{
	Data.PrevView = Data.View;
	D3DXMatrixInverse(&Data.InvView, nullptr, &View);
	OriginaView = View;
	D3DXMatrixTranspose(&Data.View, &View);
}

D3DXMATRIX ViewProjection::GetViewData()
{
	return OriginaView;
}

void ViewProjection::SetProjectionData(D3DXMATRIX View)
{
	Data.PrevProj = Data.Projection;
	OriginalProjection = View;
	D3DXMatrixTranspose(&Data.Projection, &View);
}

D3DXMATRIX ViewProjection::GetProjectionData()
{
	return OriginalProjection;
}

void ViewProjection::MapData()
{
	D3D11_MAPPED_SUBRESOURCE subResource;

	Engine->GetContext()->Map(buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
	memcpy_s(subResource.pData, ArraySize, &Data, ArraySize);
	Engine->GetContext()->Unmap(buffer.Get(), 0);
}
