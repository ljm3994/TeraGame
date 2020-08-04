#include "Framework.h"
#include "VertexBuffer.h"

VertexBuffer::VertexBuffer(void* data, UINT stride, UINT count, D3D11_USAGE BufferUsage, D3D11_CPU_ACCESS_FLAG CpuFlag, UINT BindFlag)
	: stride(stride), offset(0)
{
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = BufferUsage;
	bd.ByteWidth = stride * count;
	bd.CPUAccessFlags = CpuFlag;
	bd.BindFlags = BindFlag;

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = data;

	HRESULT hr = Engine->GetDevice()->CreateBuffer(&bd, &initData, buffer.GetAddressOf());
	DEBUG(hr);
}

VertexBuffer::VertexBuffer(UINT stride, UINT count, D3D11_USAGE BufferUsage, D3D11_CPU_ACCESS_FLAG CpuFlag, UINT BindFlag)
{
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = BufferUsage;
	bd.ByteWidth = stride * count;
	bd.CPUAccessFlags = CpuFlag;
	bd.BindFlags = BindFlag;

	HRESULT hr = Engine->GetDevice()->CreateBuffer(&bd, 0, buffer.GetAddressOf());
	DEBUG(hr);
}

VertexBuffer::~VertexBuffer()
{
}

void VertexBuffer::VSSet(UINT Slot)
{
	Engine->GetContext()->IASetVertexBuffers(Slot, 1, buffer.GetAddressOf(), &stride, &offset);
}

void VertexBuffer::SetSOTarget()
{
	Engine->GetContext()->SOSetTargets(1, buffer.GetAddressOf(), &offset);
}

void VertexBuffer::Update(void* Data, UINT ArraySize)
{
	Engine->GetContext()->UpdateSubresource(buffer.Get(), 0, nullptr, Data, stride, ArraySize);
}

void* VertexBuffer::GetMapData()
{
	Engine->GetContext()->Map(buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &BufferData);

	return BufferData.pData;
}

void VertexBuffer::unMap()
{
	Engine->GetContext()->Unmap(buffer.Get(), 0);
}
