#pragma once
#include "FrameWork\Buffer\Base\Constant.h"

class ViewDataBuffer : public Constant
{
public:
	struct Data
	{
		D3DXVECTOR4 ViewData;
	}data;

	ViewDataBuffer();
	~ViewDataBuffer();

	void SetViewData(float Width, float Height, float z, float w);
	virtual void MapData() override;
};