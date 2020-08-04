#pragma once

#include "FrameWork/Buffer/Base/Constant.h"

class WorldBuffer : public Constant
{
private:
	D3DXMATRIX Original;
	struct W
	{
		D3DXMATRIX World;
	}Data;
public:
	WorldBuffer();
	~WorldBuffer();

	void SetWorld(D3DXMATRIX value);
	D3DXMATRIX GetWorld();

	// Constant을(를) 통해 상속됨
	virtual void MapData() override;
};