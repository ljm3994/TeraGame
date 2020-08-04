#pragma once

class VertexBuffer
{
	ComPtr<ID3D11Buffer> buffer;
	D3D11_MAPPED_SUBRESOURCE BufferData;
	UINT stride;
	UINT offset;
public:
	VertexBuffer(void* data, UINT stride, UINT count, D3D11_USAGE BufferUsage = D3D11_USAGE_DEFAULT, D3D11_CPU_ACCESS_FLAG CpuFlag = (D3D11_CPU_ACCESS_FLAG)0, UINT BindFlag = D3D11_BIND_VERTEX_BUFFER);
	VertexBuffer(UINT stride, UINT count, D3D11_USAGE BufferUsage = D3D11_USAGE_DEFAULT, D3D11_CPU_ACCESS_FLAG CpuFlag = (D3D11_CPU_ACCESS_FLAG)0, UINT BindFlag = D3D11_BIND_VERTEX_BUFFER);
	~VertexBuffer();
	void VSSet(UINT Slot = 0);
	void SetSOTarget();
	void Update(void* Data, UINT ArraySize);
	ID3D11Buffer* GetBuffer() { return buffer.Get(); }
	void SetBuffer(ID3D11Buffer* value) { buffer = value; }

	void* GetMapData();
	void unMap();
};