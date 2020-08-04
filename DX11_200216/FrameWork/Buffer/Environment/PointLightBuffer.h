#pragma once
#include "FrameWork/Buffer/Base/Constant.h"
#define MAX_POINTLIGHT 32
class PointLightBuffer : public Constant
{
private:
	struct LightData
	{
		D3DXVECTOR3 Position;
		float Range;
		D3DXCOLOR Color;
	};

	struct Data
	{
		LightData lights[MAX_POINTLIGHT];
		int LightCount;

		float Padding[3];
	}data;

public:
	PointLightBuffer();
	~PointLightBuffer();

	void Add(D3DXVECTOR3 Position = D3DXVECTOR3(0.0f, 0.0f, 0.0f), float range = 20.0f, D3DXCOLOR Color = D3DXCOLOR(1, 1, 1, 1));
	LightData* GetLightData(int Index);

	// Constant을(를) 통해 상속됨
	virtual void MapData() override;

};