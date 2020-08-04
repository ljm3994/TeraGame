#pragma once

class Frustum
{
private:
	float ScreenDepth = 0.0f;
	D3DXPLANE Plane[6];
public:
	Frustum();
	~Frustum();

	void BulidFrustum();
	bool PointCheck(D3DXVECTOR3 Position);
	bool CubeCheck(D3DXVECTOR3 Position, float Radius);
	bool SphereCheck(D3DXVECTOR3 Position, float Radius);
	bool RectangleCheck(D3DXVECTOR3 Position, D3DXVECTOR3 Size);
	bool RectangleCheck(float MaxWidth, float MinWidth, float MaxHeight, float MinHeight, float MaxDepth, float MinDepth);

};