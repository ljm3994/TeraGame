#pragma once
#include "Framework/Buffer/Environment/ViewProjectionBuffer.h"
#include "FrameWork\Buffer\Environment\DirectionLightBuffer.h"
#include "FrameWork\Buffer\Environment\PointLightBuffer.h"
#include "FrameWork\Buffer\Environment\SpotLightBuffer.h"

class Application;

class Environment
{
private:
	friend class DXEngine;

	unique_ptr<DirectionLightBuffer> DirectionLight;
	unique_ptr<PointLightBuffer> PointLight;
	unique_ptr<SpotLightBuffer> SpotLight;

	D3DXMATRIX PerspectiveProjection;
	unique_ptr<ViewProjection> ViewProjectionMatrix;

	float NearZ;
	float FarZ;

public:
	Environment();
	~Environment();
	ViewProjection* GetViewProjection();
	DirectionLightBuffer* DirectionLightCreate();
	DirectionLightBuffer* GetDirectionLight();
	PointLightBuffer* PointLightCreate();
	PointLightBuffer* GetPointLight();
	SpotLightBuffer* SpotLightCreate();
	SpotLightBuffer* GetSpotLight();

	void Update();
	void Render(int ViewSlot = 1, int LightSlot = 2, int PointLightSlot = 3, int SpotLightSlot = 4);
	void SetProjection(float Width = 0, float Height = 0, float Flov = D3DX_PI * 0.25f, float NearZ = 0.1f, float FarZ = 1000.0f);
	
	D3DXMATRIX GetProjection();

	float GetNearZ() { return NearZ; }
	float GetFarZ() { return FarZ; }
};