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
	// Constant��(��) ���� ��ӵ�
	virtual void MapData() override;

	void SetColor(D3DXCOLOR color);
};