#pragma once
#include "FrameWork\Buffer\Base\ComputeBuffer.h"

class ComputeDataBuffer : public ComputeBuffer
{
private:
	void* inputData;

	UINT inputStride;
	UINT inputCount;

	UINT outputStride;
	UINT outputCount;
public:
	ComputeDataBuffer(void* inputData, UINT inputStride, UINT inputCount, UINT outputStride = 0, UINT outputCount = 0);
	~ComputeDataBuffer();

	void Copy(void* data, UINT size);

	UINT InputByteWidth() { return inputStride * inputCount; }
	UINT OutputByteWidth() { return outputStride * outputCount; }
private:
	// ComputeBuffer을(를) 통해 상속됨
	virtual void CreateInput() override;

	virtual void CreateSRV() override;

	virtual void CreateOutput() override;

	virtual void CreateUAV() override;

	virtual void CreateResult() override;

};