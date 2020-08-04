#pragma once
#include "FrameWork/Buffer/Base/Constant.h"

class BoneBuffer : public Constant
{
private:
	struct BoneData
	{
		float	Bonesweight;
		float	HasAnimation;
		float	isChanging;
		float	ChangingWeight;
		BoneData()
		{
			Bonesweight = 0.0f;
			HasAnimation = false;
			isChanging = 0.0f;
		}
	};

	struct Data
	{
		BoneData bonebuffer[MAX_INSTANCECOUNT];
		BoneData ChangeBuffer[MAX_INSTANCECOUNT];
	}data;
public:
	BoneBuffer();
	Data* GetBondData() {
		return &data;
	};
	
	// Constant을(를) 통해 상속됨
	virtual void MapData() override;
};