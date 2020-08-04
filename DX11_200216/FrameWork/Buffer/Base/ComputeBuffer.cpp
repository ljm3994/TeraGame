#include "Framework.h"
#include "ComputeBuffer.h"

ComputeBuffer::ComputeBuffer()
	: result(nullptr)
{
}

ComputeBuffer::~ComputeBuffer()
{
}

void ComputeBuffer::CreateBuffer()
{
	CreateInput();
	CreateSRV();

	CreateOutput();
	CreateUAV();

	CreateResult();
}
