#pragma once
#include "FrameWork/Buffer/Base/Constant.h"

class MaterialBuffer : public Constant
{
public:
	struct Data
	{
		D3DXCOLOR diffuse;
		D3DXCOLOR specular;
		D3DXCOLOR ambient;
		int isDiffuseMap;
		int isSpecMap;
		int isNormalMap;
		float padding;
	}data;

	MaterialBuffer() : Constant(sizeof(Data))
	{
		data.diffuse = D3DXCOLOR(1, 1, 1, 1);
		data.specular = D3DXCOLOR(1, 1, 1, 1);
		data.ambient = D3DXCOLOR(1, 1, 1, 1);
		data.isNormalMap = false;
		data.isSpecMap = false;
		data.isDiffuseMap = false;
	}

	void SetDiffuse(D3DXCOLOR value);
	void SetisDiffuseMap(bool val) { data.isDiffuseMap = val; }
	D3DXCOLOR GetDiffuse();
	void SetSpecular(D3DXCOLOR value);
	void SetisSpecMap(bool val) { data.isSpecMap = val; }
	D3DXCOLOR GetSpecular();
	void SetAmbient(D3DXCOLOR value);
	void SetisNormalMap(bool val) { data.isNormalMap = val; }
	D3DXCOLOR GetAmbient();

	// Constant을(를) 통해 상속됨
	virtual void MapData() override;
};