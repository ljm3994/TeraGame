#include "Framework.h"
#include "Camera.h"

Camera::Camera()
	:forward(0, 0, 1), right(1, 0, 0), up(0, 1, 0)
{
	Eye = { 0, 0, 0. };
	D3DXMatrixIdentity(&Viewmat);
	Rotation = { 0, 0 };
}

Camera::~Camera()
{
}

void Camera::SetView()
{
	D3DXMATRIX Temp;
	D3DXVECTOR3 AtVector;
	D3DXVECTOR3 UpVector;
	D3DXQUATERNION RotationQute;
	D3DXMATRIX Rotationmat;
	D3DXMatrixIdentity(&Viewmat);

	D3DXQuaternionRotationYawPitchRoll(&RotationQute, Rotation.y, Rotation.x, 0.0f);

	D3DXMatrixRotationQuaternion(&Rotationmat, &RotationQute);

	D3DXVec3TransformNormal(&forward, &D3DXVECTOR3(0, 0, 1), &Rotationmat);
	D3DXVec3TransformNormal(&right, &D3DXVECTOR3(1, 0, 0), &Rotationmat);
	D3DXVec3TransformNormal(&up, &D3DXVECTOR3(0, 1, 0), &Rotationmat);

	D3DXMatrixLookAtLH(&Viewmat, &Eye, &(Eye + forward), &up);
}

void Camera::SetEye(float x, float y, float z)
{
	Eye = { x, y, z };

}

D3DXVECTOR3 Camera::GetEye()
{
	return Eye;
}

void Camera::SetRotation(float x, float y)
{
	Rotation = D3DXVECTOR2(x, y);
}

D3DXVECTOR2 Camera::GetRotation()
{
	return Rotation;
}

D3DXMATRIX Camera::GetView()
{
	return Viewmat;
}

