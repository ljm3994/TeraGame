#include "Framework.h"
#include "ComputeDataBuffer.h"

ComputeDataBuffer::ComputeDataBuffer(void* inputData, UINT inputStride, UINT inputCount, UINT outputStride, UINT outputCount)
	: inputData(inputData), inputStride(inputStride), inputCount(inputCount),
	outputStride(outputStride), outputCount(outputCount)
{
	if (outputStride == 0 || outputCount == 0)
	{
		this->outputStride = inputStride;
		this->outputCount = inputCount;
	}

	CreateBuffer();
}

ComputeDataBuffer::~ComputeDataBuffer()
{
}

void ComputeDataBuffer::Copy(void* data, UINT size)
{
	Engine->GetContext()->CopyResource(result.Get(), OutPut.Get());

	D3D11_MAPPED_SUBRESOURCE subResource;
	Engine->GetContext()->Map(result.Get(), 0, D3D11_MAP_READ, 0, &subResource);

	memcpy(data, subResource.pData, size);

	Engine->GetContext()->Unmap(result.Get(), 0);
}

void ComputeDataBuffer::CreateInput()
{
	ID3D11Buffer* buffer;

	D3D11_BUFFER_DESC desc = {};
	desc.ByteWidth = InputByteWidth();
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	desc.StructureByteStride = inputStride;

	D3D11_SUBRESOURCE_DATA subResource = {};
	subResource.pSysMem = inputData;

	HRESULT hr = Engine->GetDevice()->CreateBuffer(&desc,
		inputData != nullptr ? &subResource : nullptr, &buffer);
	DEBUG(hr);

	input = (ID3D11Resource*)buffer;
}

void ComputeDataBuffer::CreateSRV()
{
	ID3D11Buffer* buffer = (ID3D11Buffer*)input.Get();

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	srvDesc.BufferEx.NumElements = inputCount;

	HRESULT hr = Engine->GetDevice()->CreateShaderResourceView(buffer, &srvDesc, &srv);
	DEBUG(hr);
}

void ComputeDataBuffer::CreateOutput()
{
	ID3D11Buffer* buffer;

	D3D11_BUFFER_DESC desc = {};
	desc.ByteWidth = OutputByteWidth();
	desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	desc.StructureByteStride = outputStride;

	HRESULT hr = Engine->GetDevice()->CreateBuffer(&desc, nullptr, &buffer);
	DEBUG(hr);

	OutPut = (ID3D11Resource*)buffer;
}

void ComputeDataBuffer::CreateUAV()
{
	ID3D11Buffer* buffer = (ID3D11Buffer*)OutPut.Get();

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.NumElements = outputCount;

	HRESULT hr = Engine->GetDevice()->CreateUnorderedAccessView(buffer, &uavDesc, &uav);
	DEBUG(hr);
}

void ComputeDataBuffer::CreateResult()
{
	ID3D11Buffer* buffer;

	D3D11_BUFFER_DESC desc;
	((ID3D11Buffer*)OutPut.Get())->GetDesc(&desc);
	desc.Usage = D3D11_USAGE_STAGING;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	desc.BindFlags = 0;
	desc.MiscFlags = 0;

	HRESULT hr = Engine->GetDevice()->CreateBuffer(&desc, nullptr, &buffer);
	DEBUG(hr);

	result = (ID3D11Resource*)buffer;
}
