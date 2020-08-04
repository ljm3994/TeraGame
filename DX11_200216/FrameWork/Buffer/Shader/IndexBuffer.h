#pragma once

class IndexBuffer
{
	ComPtr<ID3D11Buffer> buffer;

public:
	IndexBuffer(void* data, UINT count, D3D11_USAGE BufferUsage = D3D11_USAGE_DEFAULT, D3D11_CPU_ACCESS_FLAG CpuFlag = (D3D11_CPU_ACCESS_FLAG)0);
	~IndexBuffer();

	void VSSet();
};