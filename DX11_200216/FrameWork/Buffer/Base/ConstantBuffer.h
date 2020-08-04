#pragma once

template <typename type>
class ConstantBuffer
{
	type data;
	ComPtr<ID3D11Buffer> buffer;

public:
	ConstantBuffer()
	{
		CreateBuffer();
	}
	~ConstantBuffer()
	{

	}
	void CreateBuffer()
	{
		D3D11_BUFFER_DESC desc = {};

		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.ByteWidth = sizeof(data);
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		HRESULT hr = Engine->GetDevice()->CreateBuffer(&desc, nullptr, buffer.GetAddressOf());

		DEBUG(hr);
	}
	type& GetData() { return data; }
	void MapData()
	{
		D3D11_MAPPED_SUBRESOURCE subResource;

		Engine->GetContext()->Map(buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
		memcpy_s(subResource.pData, sizeof(data), &data, sizeof(data));
		Engine->GetContext()->Unmap(buffer.Get(), 0);
	}
	void SetVSBuffer(UINT slot)
	{
		MapData();
		Engine->GetContext()->VSSetConstantBuffers(slot, 1, buffer.GetAddressOf());
	}
	void SetPSBuffer(UINT slot)
	{
		MapData();
		Engine->GetContext()->PSSetConstantBuffers(slot, 1, buffer.GetAddressOf());
	}
	void SetGSBuffer(UINT slot)
	{
		MapData();
		Engine->GetContext()->GSSetConstantBuffers(slot, 1, buffer.GetAddressOf());
	}
	void SetCSBuffer(UINT slot)
	{
		MapData();
		Engine->GetContext()->CSSetConstantBuffers(slot, 1, buffer.GetAddressOf());
	}
};
