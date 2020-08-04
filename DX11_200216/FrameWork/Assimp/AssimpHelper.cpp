#include "Framework.h"
#include "AssimpHelper.h"

D3DXMATRIX AssimpHelper::ToMatrix(aiMatrix4x4 transform)
{
	D3DXMATRIX Origine = D3DXMATRIX(
		(float)transform.a1, (float)transform.b1, (float)transform.c1, (float)transform.d1,
		(float)transform.a2, (float)transform.b2, (float)transform.c2, (float)transform.d2,
		(float)transform.a3, (float)transform.b3, (float)transform.c3, (float)transform.d3,
		(float)transform.a4, (float)transform.b4, (float)transform.c4, (float)transform.d4);

	return Origine;
}

D3DXVECTOR4 AssimpHelper::ToVector4(aiVector3D val)
{
	return D3DXVECTOR4(val.x, val.y, val.z, 0.0f);
}

D3DXVECTOR3 AssimpHelper::ToVector3(aiVector3D val)
{
	return D3DXVECTOR3(val.x, val.y, val.z);
}

D3DXVECTOR2 AssimpHelper::ToVector2(aiVector3D val)
{
	return D3DXVECTOR2(val.x, val.y);
}

D3DXQUATERNION AssimpHelper::ToQuaternion(aiQuaternion val)
{
	return D3DXQUATERNION(val.x, val.y, val.z, val.w);
}
