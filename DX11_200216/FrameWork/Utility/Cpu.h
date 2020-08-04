#pragma once

class CPU
{
private:
	friend class System;
	bool isReadCpu;
	HQUERY QueryHandle;
	HCOUNTER CounterHandle;
	INT64 lastTime;
	long Usage;
	CPU();
	~CPU();

public:

	void Update();
	UINT GetPercentage();
};