#pragma once
#include "Camera.h"

class DefaultCamera : public Camera
{
public:
	DefaultCamera();
	~DefaultCamera();
	virtual void Update(float DeltaTime) override;
};