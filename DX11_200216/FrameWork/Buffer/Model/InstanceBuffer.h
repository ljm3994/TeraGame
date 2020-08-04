#pragma once
#include "FrameWork/Buffer/Base/Constant.h"

class InstanceBuffer : public Constant
{
private:
	struct Data
	{
		int isActive[MAX_INSTANCECOUNT];
	}data;
public:
	InstanceBuffer();
	~InstanceBuffer();

	void SetIsActive(UINT instanceID, bool val);
	bool GetIsActive(UINT instanceID);
	// Constant��(��) ���� ��ӵ�
	virtual void MapData() override;

};