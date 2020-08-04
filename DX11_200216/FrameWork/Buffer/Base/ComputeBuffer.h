#pragma once

class ComputeBuffer
{
protected:
	ComPtr<ID3D11Resource> input;
	ComPtr<ID3D11ShaderResourceView> srv;

	ComPtr<ID3D11Resource> OutPut;
	ComPtr<ID3D11UnorderedAccessView> uav;

	ComPtr<ID3D11Resource> result;
public:
	ComputeBuffer();
	~ComputeBuffer();

	ID3D11ShaderResourceView** GetSrv() { return srv.GetAddressOf(); }
	ID3D11UnorderedAccessView** GetUav() { return uav.GetAddressOf(); }
protected:
	void CreateBuffer();

	virtual void CreateInput() = 0;
	virtual void CreateSRV() = 0;

	virtual void CreateOutput() = 0;
	virtual void CreateUAV() = 0;

	virtual void CreateResult() = 0;
};