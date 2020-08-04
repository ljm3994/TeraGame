#include "Framework.h"
#include "Constant.h"
#include "FrameWork\Buffer\Environment\ViewProjectionBuffer.h"

Constant::Constant(UINT _ArraySize)
	:ArraySize(_ArraySize)
{
	CreateBuffer();
}

Constant::~Constant()
{
}

void Constant::CreateBuffer()
{
	D3D11_BUFFER_DESC desc = {};

	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.ByteWidth = ArraySize;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	HRESULT hr = Engine->GetDevice()->CreateBuffer(&desc, nullptr, buffer.GetAddressOf());

	DEBUG(hr);
}

void Constant::SetVSBuffer(UINT slot)
{
	MapData();
	Engine->GetContext()->VSSetConstantBuffers(slot, 1, buffer.GetAddressOf());
}

void Constant::SetPSBuffer(UINT slot)
{
	MapData();
	Engine->GetContext()->PSSetConstantBuffers(slot, 1, buffer.GetAddressOf());
}

void Constant::SetGSBuffer(UINT slot)
{
	MapData();
	Engine->GetContext()->GSSetConstantBuffers(slot, 1, buffer.GetAddressOf());
}

void Constant::SetCSBuffer(UINT slot)
{
	MapData();
	Engine->GetContext()->CSSetConstantBuffers(slot, 1, buffer.GetAddressOf());
}
