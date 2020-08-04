#include "Framework.h"
#include "IndexBuffer.h"

IndexBuffer::IndexBuffer(void* data, UINT count, D3D11_USAGE BufferUsage, D3D11_CPU_ACCESS_FLAG CpuFlag)
{
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = BufferUsage;
	bd.ByteWidth = sizeof(UINT) * count;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = CpuFlag;

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = data;

	HRESULT hr = Engine->GetDevice()->CreateBuffer(&bd, &initData, buffer.GetAddressOf());
	DEBUG(hr);
}

IndexBuffer::~IndexBuffer()
{
}

void IndexBuffer::VSSet()
{
	Engine->GetContext()->IASetIndexBuffer(buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
}
