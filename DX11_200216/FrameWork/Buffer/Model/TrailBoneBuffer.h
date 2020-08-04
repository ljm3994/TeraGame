#pragma once

#include "FrameWork/Buffer/Base/Constant.h"
#define TRAIL_BONE_MAX 200
class TrailBoneBuffer : public Constant
{
public:
	struct Data
	{
		D3DXMATRIX World[TRAIL_BONE_MAX];

		int Count;
		float Padding[3];
	}data;

	TrailBoneBuffer();
	virtual void MapData() override;
};