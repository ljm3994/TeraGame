#include "Framework.h"
#include "DefaultCamera.h"

DefaultCamera::DefaultCamera()
	: Camera()
{
}

DefaultCamera::~DefaultCamera()
{
}

void DefaultCamera::Update(float DeltaTime)
{
	SetView();
}
