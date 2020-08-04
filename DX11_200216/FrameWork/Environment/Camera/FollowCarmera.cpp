#include "Framework.h"
#include "FollowCarmera.h"

FollowCamera::FollowCamera()
	: Distance(8.0f), Height(8.0f), Offset(0, 5, 0), RotDeamping(0),
	MoveDamping(5), RotY(1.5f), RotX(0.0f), ZoomSpeed(0.01f), destPos(0, 0, 0), DestRot(0), Target(nullptr), DeltaTime(0.0f)
	, Camera(), RotSpeed(1.0f)
{
	D3DXMatrixIdentity(&RotationMat);
}

FollowCamera::~FollowCamera()
{
}

void FollowCamera::SetView()
{
	if (!Target)
		return;

	D3DXVECTOR3 Temppos = D3DXVECTOR3(0, 0, -Distance);

	if (RotDeamping > 0.0f)
	{
		D3DXQUATERNION Targetrot = Target->GetRotaion();
		if (Targetrot.y != DestRot)
		{
			DestRot = DestRot + (Targetrot.y - DestRot) * RotDeamping * DeltaTime;
		}
	}
	else
	{
		MouseControl();
		D3DXMatrixRotationYawPitchRoll(&RotationMat, RotY, RotX, 0.0f);
	}

	D3DXVec3TransformCoord(&destPos, &Temppos, &RotationMat);

	destPos += Target->GetPosition();
	destPos.y += Height;

	D3DXVec3Lerp(&Eye, &Eye, &destPos, MoveDamping * DeltaTime);

	D3DXMatrixLookAtLH(&Viewmat, &Eye, &(Target->GetPosition() + Offset), &up);
}

void FollowCamera::Update(float DeltaTime)
{
	this->DeltaTime = DeltaTime;
	SetView();
}

void FollowCamera::MouseControl()
{
	auto mstate = InputMouse->GetState();
	
	if (mevent.IsrightButton)
	{
		RotY += (mstate.x - mevent.GetLastState().x) * RotSpeed * DeltaTime;
		RotX += (mstate.y - mevent.GetLastState().y) * RotSpeed * DeltaTime;
	}

	{
		Distance -= (mstate.ScrollWheelValue - mevent.GetLastState().ScrollWheelValue) * ZoomSpeed;
		Height -= (mstate.ScrollWheelValue - mevent.GetLastState().ScrollWheelValue) * ZoomSpeed;

		if (Distance < 1.0f)
			Distance = 1.0f;
		else if (Distance > 40.0f)
			Distance = 40.0f;

		if (Height < 1.0f)
			Height = 1.0f;
		else if (Height > 40.0f)
			Height = 40.0f;
	}

	mevent.Update(mstate);
}

void FollowCamera::SetTarget(Transform* transform, float DeltaTime)
{
	Target = transform;
	Update(DeltaTime);
}
