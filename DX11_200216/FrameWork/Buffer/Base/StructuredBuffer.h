#pragma once

template<typename type>
class StructureBuffer
{
private:
	UINT Elements;
	ComPtr<ID3D11Buffer> buffer;
	ComPtr<ID3D11ShaderResourceView> srv;
	ComPtr<ID3D11UnorderedAccessView> uav;

public:
	StructureBuffer(UINT Elements, UINT bineFlag = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS, bool Dynamic = false,
		bool AppendConsume = false, bool HiddenCounter = false)
		: Elements(Elements), buffer(nullptr), srv(nullptr), uav(nullptr)
	{
		D3D11_BUFFER_DESC bufferDesc;
		bufferDesc.ByteWidth = sizeof(type) * Elements;
		bufferDesc.Usage = Dynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
		bufferDesc.BindFlags = bineFlag;
		bufferDesc.CPUAccessFlags = Dynamic ? D3D11_CPU_ACCESS_WRITE : 0;
		bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		bufferDesc.StructureByteStride = sizeof(type);

		HRESULT hr;

		hr = Engine->GetDevice()->CreateBuffer(&bufferDesc, nullptr, buffer.GetAddressOf());

		DEBUG(hr);
		if (bineFlag & D3D11_BIND_SHADER_RESOURCE)
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
			srvDesc.Format = DXGI_FORMAT_UNKNOWN;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
			srvDesc.Buffer.FirstElement = 0;
			srvDesc.Buffer.NumElements = Elements;

			hr = Engine->GetDevice()->CreateShaderResourceView(buffer.Get(), &srvDesc, srv.GetAddressOf());

			DEBUG(hr);
		}

		if (bineFlag & D3D11_BIND_UNORDERED_ACCESS)
		{
			D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
			uavDesc.Format = DXGI_FORMAT_UNKNOWN;
			uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
			uavDesc.Buffer.FirstElement = 0;
			uavDesc.Buffer.NumElements = Elements;
			uavDesc.Buffer.Flags = 0;
			uavDesc.Buffer.Flags |= AppendConsume ? D3D11_BUFFER_UAV_FLAG_APPEND : 0;
			uavDesc.Buffer.Flags |= HiddenCounter ? D3D11_BUFFER_UAV_FLAG_COUNTER : 0;

			hr = Engine->GetDevice()->CreateUnorderedAccessView(buffer.Get(), &uavDesc, uav.GetAddressOf());

			DEBUG(hr);
		}
	}
	~StructureBuffer()
	{

	}

	ID3D11Buffer* GetBuffer() const { return buffer.Get(); }
	ID3D11ShaderResourceView* GetSRV() const { return srv.Get(); }
	ID3D11UnorderedAccessView* GetUAV() const { return uav.Get(); }

	type* Discard()
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		Engine->GetContext()->Map(buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

		return static_cast<type*>(mappedResource.pData);
	}
	void UnMap()
	{
		Engine->GetContext()->Unmap(buffer.Get(), 0);
	}
};
