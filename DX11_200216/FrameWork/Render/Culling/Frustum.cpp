#include "Framework.h"
#include "Frustum.h"

Frustum::Frustum()
{
}

Frustum::~Frustum()
{
}

void Frustum::BulidFrustum()
{
	D3DXMATRIX projection = Engine->GetProjectionMatrix();
	D3DXMATRIX viewMat = Engine->GetCurrentCamera()->GetView();

	D3DXMATRIX FinalMat;
	D3DXMatrixMultiply(&FinalMat, &viewMat, &projection);

	// 절두체의 가까운 평면 계산
	Plane[0].a = FinalMat._14 + FinalMat._13;
	Plane[0].b = FinalMat._24 + FinalMat._23;
	Plane[0].c = FinalMat._34 + FinalMat._33;
	Plane[0].d = FinalMat._44 + FinalMat._43;

	// 절두체의 면 평면 계산
	Plane[1].a = FinalMat._14 - FinalMat._13;
	Plane[1].b = FinalMat._24 - FinalMat._23;
	Plane[1].c = FinalMat._34 - FinalMat._33;
	Plane[1].d = FinalMat._44 - FinalMat._43;

	// 절두체의 왼쪽 평면 계산
	Plane[2].a = FinalMat._14 + FinalMat._11;
	Plane[2].b = FinalMat._24 + FinalMat._21;
	Plane[2].c = FinalMat._34 + FinalMat._31;
	Plane[2].d = FinalMat._44 + FinalMat._41;

	// 절두체의 오른쪽 평면 계산
	Plane[3].a = FinalMat._14 - FinalMat._11;
	Plane[3].b = FinalMat._24 - FinalMat._21;
	Plane[3].c = FinalMat._34 - FinalMat._31;
	Plane[3].d = FinalMat._44 - FinalMat._41;

	// 절두체의 꼭대기 계산
	Plane[4].a = FinalMat._14 - FinalMat._12;
	Plane[4].b = FinalMat._24 - FinalMat._22;
	Plane[4].c = FinalMat._34 - FinalMat._32;
	Plane[4].d = FinalMat._44 - FinalMat._42;

	// 절두체의 바닥 계산
	Plane[5].a = FinalMat._14 + FinalMat._12;
	Plane[5].b = FinalMat._24 + FinalMat._22;
	Plane[5].c = FinalMat._34 + FinalMat._32;
	Plane[5].d = FinalMat._44 + FinalMat._42;

	// 모든 면 표준화
	for (int i = 0; i < 6; i++)
		D3DXPlaneNormalize(&Plane[i], &Plane[i]);
}

bool Frustum::PointCheck(D3DXVECTOR3 Position)
{
	for (int i = 0; i < 6; i++)
	{
		float Dot = D3DXPlaneDotCoord(&Plane[i], &Position);
		if (Dot < 0.0f)
			return false;
	}

	return true;
}

bool Frustum::CubeCheck(D3DXVECTOR3 Position, float Radius)
{
	float Dot;
	D3DXVECTOR3 edge;
	for (int i = 0; i < 6; i++)
	{
		edge.x = Position.x - Radius;
		edge.y = Position.y - Radius;
		edge.z = Position.z - Radius;
		Dot = D3DXPlaneDotCoord(&Plane[i], &edge);
		if (Dot > 0.0f)
			continue;

		edge.x = Position.x + Radius;
		edge.y = Position.y - Radius;
		edge.z = Position.z - Radius;
		Dot = D3DXPlaneDotCoord(&Plane[i], &edge);
		if (Dot > 0.0f)
			continue;

		edge.x = Position.x + Radius;
		edge.y = Position.y + Radius;
		edge.z = Position.z - Radius;
		Dot = D3DXPlaneDotCoord(&Plane[i], &edge);
		if (Dot > 0.0f)
			continue;

		edge.x = Position.x - Radius;
		edge.y = Position.y - Radius;
		edge.z = Position.z + Radius;
		Dot = D3DXPlaneDotCoord(&Plane[i], &edge);
		if (Dot > 0.0f)
			continue;

		edge.x = Position.x + Radius;
		edge.y = Position.y - Radius;
		edge.z = Position.z + Radius;
		Dot = D3DXPlaneDotCoord(&Plane[i], &edge);
		if (Dot > 0.0f)
			continue;

		edge.x = Position.x - Radius;
		edge.y = Position.y + Radius;
		edge.z = Position.z + Radius;
		Dot = D3DXPlaneDotCoord(&Plane[i], &edge);
		if (Dot > 0.0f)
			continue;

		edge.x = Position.x - Radius;
		edge.y = Position.y + Radius;
		edge.z = Position.z - Radius;
		Dot = D3DXPlaneDotCoord(&Plane[i], &edge);
		if (Dot > 0.0f)
			continue;

		edge.x = Position.x + Radius;
		edge.y = Position.y + Radius;
		edge.z = Position.z + Radius;
		Dot = D3DXPlaneDotCoord(&Plane[i], &edge);
		if (Dot > 0.0f)
			continue;

		return false;
	}
	return true;
}

bool Frustum::SphereCheck(D3DXVECTOR3 Position, float Radius)
{
	for (int i = 0; i < 6; i++)
	{
		float Dot = D3DXPlaneDotCoord(&Plane[i], &Position);

		if (Dot <= -Radius)
			return false;
	}

	return true;
}

bool Frustum::RectangleCheck(D3DXVECTOR3 Position, D3DXVECTOR3 Size)
{
	float Dot;
	D3DXVECTOR3 line;
	for (int i = 0; i < 6; i++)
	{
		line.x = Position.x - Size.x;
		line.y = Position.y - Size.y;
		line.z = Position.z - Size.z;
		Dot = D3DXPlaneDotCoord(&Plane[i], &line);

		if (Dot >= 0.0f)
			continue;

		line.x = Position.x + Size.x;
		line.y = Position.y - Size.y;
		line.z = Position.z - Size.z;
		Dot = D3DXPlaneDotCoord(&Plane[i], &line);

		if (Dot >= 0.0f)
			continue;

		line.x = Position.x - Size.x;
		line.y = Position.y + Size.y;
		line.z = Position.z - Size.z;
		Dot = D3DXPlaneDotCoord(&Plane[i], &line);

		if (Dot >= 0.0f)
			continue;

		line.x = Position.x + Size.x;
		line.y = Position.y + Size.y;
		line.z = Position.z - Size.z;
		Dot = D3DXPlaneDotCoord(&Plane[i], &line);

		if (Dot >= 0.0f)
			continue;

		line.x = Position.x - Size.x;
		line.y = Position.y - Size.y;
		line.z = Position.z + Size.z;
		Dot = D3DXPlaneDotCoord(&Plane[i], &line);

		if (Dot >= 0.0f)
			continue;

		line.x = Position.x + Size.x;
		line.y = Position.y - Size.y;
		line.z = Position.z + Size.z;
		Dot = D3DXPlaneDotCoord(&Plane[i], &line);

		if (Dot >= 0.0f)
			continue;

		line.x = Position.x - Size.x;
		line.y = Position.y + Size.y;
		line.z = Position.z + Size.z;
		Dot = D3DXPlaneDotCoord(&Plane[i], &line);

		if (Dot >= 0.0f)
			continue;

		line.x = Position.x + Size.x;
		line.y = Position.y + Size.y;
		line.z = Position.z + Size.z;
		Dot = D3DXPlaneDotCoord(&Plane[i], &line);

		if (Dot >= 0.0f)
			continue;

		return false;
	}

	return true;
}

bool Frustum::RectangleCheck(float MaxWidth, float MinWidth, float MaxHeight, float MinHeight, float MaxDepth, float MinDepth)
{
	float Dot = 0.0f;

	for (int i = 0; i < 6; i++)
	{
		Dot = D3DXPlaneDotCoord(&Plane[i], &D3DXVECTOR3(MinWidth, MinHeight, MinDepth));
		if (Dot >= 0.0f)
			continue;

		Dot = D3DXPlaneDotCoord(&Plane[i], &D3DXVECTOR3(MaxWidth, MinHeight, MinDepth));
		if (Dot >= 0.0f)
			continue;

		Dot = D3DXPlaneDotCoord(&Plane[i], &D3DXVECTOR3(MinWidth, MaxHeight, MinDepth));
		if (Dot >= 0.0f)
			continue;

		Dot = D3DXPlaneDotCoord(&Plane[i], &D3DXVECTOR3(MaxWidth, MaxHeight, MinDepth));
		if (Dot >= 0.0f)
			continue;

		Dot = D3DXPlaneDotCoord(&Plane[i], &D3DXVECTOR3(MinWidth, MinHeight, MaxDepth));
		if (Dot >= 0.0f)
			continue;

		Dot = D3DXPlaneDotCoord(&Plane[i], &D3DXVECTOR3(MaxWidth, MinHeight, MaxDepth));
		if (Dot >= 0.0f)
			continue;

		Dot = D3DXPlaneDotCoord(&Plane[i], &D3DXVECTOR3(MinWidth, MaxHeight, MaxDepth));
		if (Dot >= 0.0f)
			continue;

		Dot = D3DXPlaneDotCoord(&Plane[i], &D3DXVECTOR3(MaxWidth, MaxHeight, MaxDepth));
		if (Dot >= 0.0f)
			continue;

		return false;
	}

	return true;
}
