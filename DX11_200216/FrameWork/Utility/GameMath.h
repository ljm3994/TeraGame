#pragma once

namespace GameMath
{
	struct Vector3 : public D3DXVECTOR3
	{
		Vector3() {};
		Vector3(CONST FLOAT* val) : D3DXVECTOR3(val) {}
		Vector3(CONST D3DVECTOR& val) : D3DXVECTOR3(val) {}
		Vector3(CONST D3DXFLOAT16* val) : D3DXVECTOR3(val) {}
		Vector3(FLOAT x, FLOAT y, FLOAT z) : D3DXVECTOR3(x, y, z) {}
		Vector3 operator- (const Vector3& rhs)const { return { x - rhs.x, y - rhs.y, z - rhs.z }; }
		float operator* (const Vector3& rhs)const { return { x * rhs.x + y * rhs.y + z * rhs.z }; }
		Vector3 operator^ (const Vector3& rhs)const { return { y * rhs.z - z * rhs.y, z * rhs.x - x * rhs.z, x * rhs.y - y * rhs.x }; }
		Vector3 operator* (const float& rhs)const { return { x * rhs, y * rhs, z * rhs }; }
	};

	float Random(const int& min, const int& max);
	float Random(const float& min, const float& max);
}