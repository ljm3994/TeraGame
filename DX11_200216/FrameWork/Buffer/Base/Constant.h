#pragma once

class Constant
{
protected:
	ComPtr<ID3D11Buffer> buffer;

	UINT ArraySize;
protected:

	Constant(UINT _ArraySize);
	virtual ~Constant();
	void CreateBuffer();

public:
	virtual void MapData() = 0;
	void SetVSBuffer(UINT slot);
	void SetPSBuffer(UINT slot);
	void SetGSBuffer(UINT slot);
	void SetCSBuffer(UINT slot);
};