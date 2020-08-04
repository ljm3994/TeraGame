#pragma once
#include "FrameWork/Buffer/Base/Constant.h"

class ViewProjection : public Constant
{
private:
	struct VP
	{
		D3DXMATRIX View;
		D3DXMATRIX Projection;
		D3DXMATRIX InvView;
		D3DXMATRIX PrevView;
		D3DXMATRIX PrevProj;
	}Data;
	D3DXMATRIX OriginaView;
	D3DXMATRIX OriginalProjection;
public:
	ViewProjection();
	~ViewProjection();
	void SetViewData(D3DXMATRIX View);
	D3DXMATRIX GetViewData();
	void SetProjectionData(D3DXMATRIX View);
	D3DXMATRIX GetProjectionData();

	// Constant을(를) 통해 상속됨
	virtual void MapData() override;
};