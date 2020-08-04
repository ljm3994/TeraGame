#pragma once
#include "Camera.h"

class FreeCarmera : public Camera
{
protected:
	float moveSpeed;
	float rotationSpeed;
	Mouse::MouseEvent mevent;
public:
	FreeCarmera();
	~FreeCarmera();
	virtual void Update(float DeltaTime) override;
	bool CamMove(float DeltaTime);
};