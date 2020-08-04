#include "Framework.h"
#include "AssimpType.h"

D3DXMATRIX AnimationCurve::Evaluate(float DeltaTime, int Duration)
{
	D3DXVECTOR3 pos = CalcInterpolatedPosition(DeltaTime, Duration);
	D3DXVECTOR3 Scale = CalcInterpolatedScale(DeltaTime, Duration);
	D3DXQUATERNION rot = CalcInterpolatedRotation(DeltaTime, Duration);

	D3DXMATRIX S, R, T;

	D3DXMatrixScaling(&S, Scale.x, Scale.y, Scale.z);
	D3DXMatrixTranslation(&T, pos.x, pos.y, pos.z);
	D3DXMatrixRotationQuaternion(&R, &rot);
	return S * R * T;
}

D3DXVECTOR3 AnimationCurve::CalcInterpolatedPosition(float DeltaTime, int Duration)
{
	if (PositionKeys.empty())
		return D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	if (PositionKeys.size() == 1)
		return PositionKeys.front().val;

	if (DeltaTime == Duration - 1)
	{
		return PositionKeys.back().val;
	}

	int frame = 0;
	while (frame < ScaleKeys.size() - 1)
	{
		if (DeltaTime < ScaleKeys[frame + 1].Time)
		{
			break;
		}

		frame++;
	}

	if (frame >= PositionKeys.size() - 1)
		return PositionKeys.back().val;

	auto Next = frame + 1;

	float difftime = (float)PositionKeys[Next].Time - PositionKeys[frame].Time;
	float factor = (DeltaTime - (float)(PositionKeys[frame].Time)) / difftime;
	if (factor < 0.0f)
		return PositionKeys.front().val;

	D3DXVECTOR3 start = PositionKeys[frame].val;
	D3DXVECTOR3 end = PositionKeys[Next].val;

	return start + factor * (end - start);
}

D3DXVECTOR3 AnimationCurve::CalcInterpolatedScale(float DeltaTime, int Duration)
{
	if (ScaleKeys.empty())
		return D3DXVECTOR3(1.0f, 1.0f, 1.0f);

	if (ScaleKeys.size() == 1)
		return ScaleKeys.front().val;

	if (DeltaTime == Duration - 1)
	{
		return ScaleKeys.back().val;
	}

	int frame = 0;
	while (frame < ScaleKeys.size() - 1)
	{
		if (DeltaTime < ScaleKeys[frame + 1].Time)
		{
			break;
		}

		frame++;
	}

	if (frame >= ScaleKeys.size() - 1)
		return ScaleKeys.back().val;

	auto Next = frame + 1;

	float difftime = (float)ScaleKeys[Next].Time - ScaleKeys[frame].Time;
	float factor = (DeltaTime - (float)(ScaleKeys[frame].Time)) / difftime;
	if (factor < 0.0f)
		return ScaleKeys.front().val;

	D3DXVECTOR3 start = ScaleKeys[frame].val;
	D3DXVECTOR3 end = ScaleKeys[Next].val;

	return start + factor * (end - start);
}

D3DXQUATERNION AnimationCurve::CalcInterpolatedRotation(float DeltaTime, int Duration)
{
	if (RotationKeys.empty())
		return D3DXQUATERNION(0.0f, 0.0f, 0.0f, 1.0f);

	if (RotationKeys.size() == 1)
		return RotationKeys.front().val;

	if (DeltaTime == Duration - 1)
	{
		return RotationKeys.back().val;
	}

	int frame = 0;
	while (frame < RotationKeys.size() - 1)
	{
		if (DeltaTime < RotationKeys[frame + 1].Time)
		{
			break;
		}

		frame++;
	}

	if (frame >= RotationKeys.size() - 1)
		return RotationKeys.back().val;

	auto Next = frame + 1;

	float difftime = (float)RotationKeys[Next].Time - RotationKeys[frame].Time;
	float factor = (DeltaTime - (float)(RotationKeys[frame].Time)) / difftime;
	if (factor < 0.0f)
		return RotationKeys.front().val;

	D3DXQUATERNION start = RotationKeys[frame].val;
	D3DXQUATERNION end = RotationKeys[Next].val;

	D3DXQuaternionSlerp(&start, &start, &end, factor);
	return start;
}
