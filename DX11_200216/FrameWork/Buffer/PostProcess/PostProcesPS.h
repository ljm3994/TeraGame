#pragma once
#include "FrameWork\Buffer\Base\Constant.h"
#define MAX_INPUT 4
class PostProcesPS : public Constant
{
public:
	struct Data
	{
		D3DXVECTOR2 InputSize[MAX_INPUT];
		D3DXVECTOR2 OutputSize;
		D3DXVECTOR2 pad;
	}data;

	PostProcesPS();
	~PostProcesPS();

	// Constant을(를) 통해 상속됨
	virtual void MapData() override;
};