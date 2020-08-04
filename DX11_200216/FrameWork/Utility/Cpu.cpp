#include "Framework.h"
#include "Cpu.h"

CPU::CPU()
{
	PDH_STATUS status;

	isReadCpu = true;

	status = PdhOpenQuery(NULL, 0, &QueryHandle);
	if (status != ERROR_SUCCESS)
	{
		isReadCpu = false;
	}

	status = PdhAddCounter(QueryHandle, L"\\Processor(_Total)\\% processor time", 0, &CounterHandle);

	if (status != ERROR_SUCCESS)
	{
		isReadCpu = false;
	}

	lastTime = GetTickCount64();

	Usage = 0;
}

CPU::~CPU()
{
	// �������� �ڵ��� �ݾ���� �Ѵ�.
	if (isReadCpu)
	{
		PdhCloseQuery(QueryHandle);
	}
}

void CPU::Update()
{
	PDH_FMT_COUNTERVALUE value;
	// 1�ʸ��� ��뷮�� üũ�Ѵ�
	if (isReadCpu)
	{
		if ((lastTime + 1000) < GetTickCount64())
		{
			lastTime = GetTickCount64();

			PdhCollectQueryData(QueryHandle);

			PdhGetFormattedCounterValue(CounterHandle, PDH_FMT_LONG, NULL, &value);

			Usage = value.longValue;
		}
	}
}

UINT CPU::GetPercentage()
{
	if (isReadCpu)
	{
		return (UINT)Usage;
	}
	else
	{
		return 0;
	}
}
