#pragma once

class AssimpHelper
{
public:
	static D3DXMATRIX ToMatrix(aiMatrix4x4 transform);
	static D3DXVECTOR4 ToVector4(aiVector3D val);
	static D3DXVECTOR3 ToVector3(aiVector3D val);
	static D3DXVECTOR2 ToVector2(aiVector3D val);
	static D3DXQUATERNION ToQuaternion(aiQuaternion val);
};