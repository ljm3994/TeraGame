#pragma once
#include "FrameWork/Buffer/Base/Constant.h"
#define MAX_SPOTLIGHT 32
class SpotLightBuffer : public Constant
{
private:
	struct SpotData
	{
		D3DXCOLOR Color;
		D3DXVECTOR3 Position;
		float Range;
		D3DXVECTOR3 Direction;
		float Outer;
		float Inner;

		float padding[3];
	};

	struct Data
	{
		SpotData lights[MAX_SPOTLIGHT];
		int LightCount;

		float Padding[3];
	}data;

public:
	SpotLightBuffer();
	~SpotLightBuffer();

	void Add(D3DXCOLOR Color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), float Range = 60.0f, D3DXVECTOR3 Pos = D3DXVECTOR3(0, 8.0f, 0), D3DXVECTOR3 Dir = D3DXVECTOR3(0, -1, 0), float Outer = 65.0f, float inner = 55.0f);
	SpotData* GetSpotData(int Index);
	// Constant을(를) 통해 상속됨
	virtual void MapData() override;

};