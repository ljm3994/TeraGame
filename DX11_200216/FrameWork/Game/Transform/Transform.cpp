#include "Framework.h"
#include "Transform.h"

Transform::Transform(string tag)
	: tag(tag), LocalPosition(0, 0, 0), rotation(0, 0, 0, 1.0f), scale(1, 1, 1), RotationPivot(0, 0, 0), ScalePivot(0, 0, 0), forward(0, 0, 1), right(1, 0, 0), up(0, 1, 0)
{
	OffsetPosition = { 0.0f, 0.0f, 0.0f };
	YawAngle = 0.0f;
	PitchAngle = 0.0f;
	RollAngle = 0.0f;
	ScaleFactor = 1.0f;
	isUpdate = true;
	D3DXMatrixIdentity(&world);
	D3DXMatrixIdentity(&RotMat);
	worldBuffer = make_unique<WorldBuffer>(WorldBuffer());
}

Transform::~Transform()
{
}

void Transform::Init()
{

}

void Transform::UpdateWorld()
{
	YawRotation();
	PitchRotation();
	RollRotation();
	scale *= ScaleFactor;
	D3DXMatrixTransformation(&world, &ScalePivot, nullptr, &scale, &RotationPivot, &rotation, &LocalPosition);

	if (parent != nullptr)
	{
		world *= *parent;
	}

	WorldPosition = Vector3(world._41, world._42, world._43);

	worldBuffer->SetWorld(world);
}

void Transform::SetVS(UINT slot)
{
	worldBuffer->SetVSBuffer(slot);
}

void Transform::SetPosition(float x, float y, float z)
{
	LocalPosition = { x, y, z };
}

void Transform::SetScale(float x, float y, float z)
{
	scale = { x, y, z };
}

void Transform::SetRotation(float x, float y, float z, float w)
{
	rotation = { x, y, z, w };
}
// Y축 회전 함수
void Transform::YawRotation()
{
	D3DXQUATERNION qRot;     // 회전
	D3DXQuaternionRotationAxis(&qRot, &up, YawAngle);     //해당 각도만큼 회전 시킨다
	rotation = rotation * qRot;         // 현재 회전값이랑 곱한다.

	D3DXMATRIX mat;
	D3DXMatrixRotationQuaternion(&mat, &qRot);   //회전 매트릭스 구한다.

	D3DXVec3TransformNormal(&forward, &forward, &mat);  //포워드 벡터 노말라이즈
	D3DXVec3TransformNormal(&right, &right, &mat);      //라이트 벡터 노말라이즈
}

void Transform::RollRotation()
{
	D3DXQUATERNION qRot;
	D3DXQuaternionRotationAxis(&qRot, &forward, RollAngle);
	rotation = rotation * qRot;

	D3DXMATRIX mat;
	D3DXMatrixRotationQuaternion(&mat, &qRot);

	D3DXVec3TransformNormal(&up, &up, &mat);
	D3DXVec3TransformNormal(&right, &right, &mat);
}

void Transform::PitchRotation()
{
	D3DXQUATERNION qRot;
	D3DXQuaternionRotationAxis(&qRot, &right, PitchAngle);
	rotation = rotation * qRot;

	D3DXMATRIX mat;
	D3DXMatrixRotationQuaternion(&mat, &qRot);

	D3DXVec3TransformNormal(&up, &up, &mat);
	D3DXVec3TransformNormal(&forward, &forward, &mat);
}

void Transform::SetScalePivot(float x, float y, float z)
{
	ScalePivot = { x, y, z };
}

void Transform::SetScaleFactor(float value)
{
	ScaleFactor = value;
}

void Transform::SetRotationPivot(float x, float y, float z)
{
	RotationPivot = { x, y, z };
}

void Transform::SetOffSetPosition(float x, float y, float z)
{
	OffsetPosition = { x, y, z };
}

bool Transform::Move(float DeltaTime, D3DXVECTOR3 TagetPos)
{
	float dist = sqrt(pow(LocalPosition.x - TagetPos.x, 2.0f) + pow(LocalPosition.z - TagetPos.z, 2.0f));

	if (dist >= 0.01f)
	{
		D3DXVECTOR2 RePos = D3DXVECTOR2(TagetPos.x - LocalPosition.x, TagetPos.z - LocalPosition.z);
		D3DXVec2Normalize(&RePos, &RePos);
		//러프로도 이동은 되지만 뭔가 깔끔하게 이동되지를 않음......
		//D3DXVec2Lerp(&RePos, &D3DXVECTOR2(position.x, position.z), &D3DXVECTOR2(TagetPos.x, TagetPos.z), 0.4f * DELTATIME);
		LocalPosition.x += RePos.x * 10.0f * DeltaTime;
		LocalPosition.z += RePos.y * 10.0f * DeltaTime;
		return true;
	}

	return false;
}

void Transform::Turn(D3DXVECTOR3 TagetPos)
{
}

bool Transform::GetIsUpdate()
{
	return false;
}

void Transform::SetIsUpdate(bool value)
{
}

void Transform::SetParent(D3DXMATRIX* val)
{
	parent = val;
}

void Transform::SetWorld(D3DXMATRIX val)
{
	//D3DXMatrixInverse(&val, nullptr, &val);
	D3DXMatrixDecompose(&scale, &rotation, &LocalPosition, &val);
	worldBuffer.get()->SetWorld(val);
}

D3DXMATRIX* Transform::GetWorld()
{
	return &world;
}

D3DXVECTOR3 Transform::GetPosition()
{
	return LocalPosition;
}

D3DXVECTOR3 Transform::GetScale()
{
	return scale;
}

D3DXQUATERNION Transform::GetRotaion()
{
	return rotation;
}

D3DXVECTOR3 Transform::GetOffSetPosition()
{
	return OffsetPosition;
}
