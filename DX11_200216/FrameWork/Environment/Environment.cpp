#include "Framework.h"
#include "FrameWork/Environment/Camera/FreeCamera.h"

Environment::Environment()
{
	ViewProjectionMatrix = make_unique<ViewProjection>(ViewProjection());
	NearZ = 0;
	FarZ = 0;
}

Environment::~Environment()
{
}

ViewProjection* Environment::GetViewProjection()
{
	
	return ViewProjectionMatrix.get();
}

DirectionLightBuffer* Environment::DirectionLightCreate()
{
	DirectionLight.reset(new DirectionLightBuffer());

	return DirectionLight.get();
}

DirectionLightBuffer* Environment::GetDirectionLight()
{
	return DirectionLight.get();
}

PointLightBuffer* Environment::PointLightCreate()
{
	PointLight.reset(new PointLightBuffer());
	return PointLight.get();
}

PointLightBuffer* Environment::GetPointLight()
{
	return PointLight.get();
}

SpotLightBuffer* Environment::SpotLightCreate()
{
	SpotLight.reset(new SpotLightBuffer());
	return SpotLight.get();
}

SpotLightBuffer* Environment::GetSpotLight()
{
	return SpotLight.get();
}

void Environment::Update()
{
	ViewProjectionMatrix->SetViewData(Engine->GetCurrentCamera()->GetView());
	SetProjection(Engine->GetClientWidth(), Engine->GetClientHeight());
}

void Environment::Render(int ViewSlot, int LightSlot, int PointLightSlot, int SpotLightSlot)
{
	ViewProjectionMatrix->SetVSBuffer(ViewSlot);

	if(DirectionLight)
		DirectionLight->SetPSBuffer(LightSlot);
	if (PointLight)
		PointLight->SetPSBuffer(PointLightSlot);
	if (SpotLight)
		SpotLight->SetPSBuffer(SpotLightSlot);
}


void Environment::SetProjection(float Width, float Height, float Flov, float NearZ, float FarZ)
{
	D3DXMatrixIdentity(&PerspectiveProjection);
	D3DXMatrixPerspectiveFovLH(&PerspectiveProjection, Flov, Width / Height, NearZ, FarZ);
	
	this->NearZ = NearZ;
	this->FarZ = FarZ;

	ViewProjectionMatrix->SetProjectionData(PerspectiveProjection);
}

D3DXMATRIX Environment::GetProjection()
{
	return ViewProjectionMatrix->GetProjectionData();
}
