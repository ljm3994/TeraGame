#pragma once
#include "Camera.h"

class FollowCamera : public Camera
{
private:
	float Distance;
	float Height;
	D3DXVECTOR3 Offset;

	D3DXVECTOR3 destPos;
	float DestRot;

	float MoveDamping;
	float RotDeamping;

	float RotY;
	float RotX;
	float RotSpeed;

	float ZoomSpeed;
	float DeltaTime;

	Transform* Target;

	D3DXMATRIX RotationMat;
	Mouse::MouseEvent mevent;
public:
	FollowCamera();
	~FollowCamera();

	void SetView() override;
	virtual void Update(float DeltaTime) override;

	void MouseControl();

	void SetTarget(Transform* transform, float DeltaTime);
};