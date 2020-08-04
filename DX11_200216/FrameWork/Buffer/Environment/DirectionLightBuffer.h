#pragma once
#include "FrameWork/Buffer/Base/Constant.h"
class DirectionLightBuffer : public Constant
{
private:
	struct Data
	{
		D3DXVECTOR3 LightDirection;
		float SpecExp;

		D3DXCOLOR AmbientLight;
	}data;

public:
	DirectionLightBuffer();
	~DirectionLightBuffer();
	D3DXVECTOR3 GetDirection() { return data.LightDirection; }
	void SetDirection(D3DXVECTOR3 Direction);
	void SetSpecExp(float val);
	void SetAmbientLight(D3DXCOLOR Ambient);

	// Constant을(를) 통해 상속됨
	virtual void MapData() override;
};