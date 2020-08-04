#include "Framework.h"
#include "FreeCamera.h"

FreeCarmera::FreeCarmera() : Camera(), moveSpeed(30), rotationSpeed(1.0f)
{
}

FreeCarmera::~FreeCarmera()
{
}

void FreeCarmera::Update(float DeltaTime)
{
	if (CamMove(DeltaTime))
	{
		
	}
	SetView();
}

bool FreeCarmera::CamMove(float DeltaTime)
{
	bool Move = false;
	auto mstate = InputMouse->GetState();
	auto kstate = InputKey->GetState();

	if (mstate.IsrightButton)
	{
		if (kstate.W)
		{
			Eye += forward * moveSpeed * DeltaTime;
			Move = true;
		}
		else if (kstate.S)
		{
			Eye -= forward * moveSpeed * DeltaTime;
			Move = true;
		}
		else if (kstate.A)
		{
			Eye -= right * moveSpeed * DeltaTime;
			Move = true;
		}
		else if (kstate.D)
		{
			Eye += right * moveSpeed * DeltaTime;
			Move = true;
		}
		else if (kstate.Q)
		{
			Eye += up * moveSpeed * DeltaTime;
			Move = true;
		}
		else if (kstate.E)
		{
			Eye -= up * moveSpeed * DeltaTime;
			Move = true;
		}

		Rotation.x += (mstate.y - mevent.GetLastState().y) * rotationSpeed * DeltaTime;
		Rotation.y += (mstate.x - mevent.GetLastState().x) * rotationSpeed * DeltaTime;

		Move = true;
	}
	else
	{

	}
	mevent.Update(mstate);
	return Move;
}
