#pragma once
#include "FrameWork/Buffer/Base/Constant.h"
class RayBuffer : public Constant
{
private:
	struct Data
	{
		D3DXVECTOR3 position;
		UINT size;

		D3DXVECTOR3 direction;
		float padding;
	}data;
public:
	RayBuffer();
	~RayBuffer();
	void SetRay(const Ray& ray);
	void SetSize(UINT Size);
	virtual void MapData() override;
};