#include "Framework.h"
#include "MaterialBuffer.h"

void MaterialBuffer::SetDiffuse(D3DXCOLOR value)
{
	data.diffuse = value;
}

D3DXCOLOR MaterialBuffer::GetDiffuse()
{
	return data.diffuse;
}

void MaterialBuffer::SetSpecular(D3DXCOLOR value)
{
	data.specular = value;
}

D3DXCOLOR MaterialBuffer::GetSpecular()
{
	return data.specular;
}

void MaterialBuffer::SetAmbient(D3DXCOLOR value)
{
	data.ambient = value;
}

D3DXCOLOR MaterialBuffer::GetAmbient()
{
	return data.ambient;
}

void MaterialBuffer::MapData()
{
	D3D11_MAPPED_SUBRESOURCE subResource;

	Engine->GetContext()->Map(buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
	memcpy_s(subResource.pData, ArraySize, &data, ArraySize);
	Engine->GetContext()->Unmap(buffer.Get(), 0);
}
