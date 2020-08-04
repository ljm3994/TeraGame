#pragma once
#include "FrameWork/Buffer/Base/Constant.h"
class TrailColorBuffer : public Constant
{
private:
	struct Data
	{
		D3DXCOLOR Color;
	}data;

public:
	TrailColorBuffer();
	// Constant을(를) 통해 상속됨
	virtual void MapData() override;

	void SetColor(D3DXCOLOR color);
};