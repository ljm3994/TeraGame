#pragma once

enum class CarmeraType
{
	DEFAULTCARMERA,
	FREECARMERA,
	FPSCARMERA,
	FOLLOWCAMERA
};

class Camera
{
protected:
	friend class DXEngine;
	D3DXVECTOR3 Eye;
	D3DXVECTOR3 forward;
	D3DXVECTOR3 right;
	D3DXVECTOR3 up;

	D3DXVECTOR2 Rotation;
	D3DXMATRIX Viewmat;

public:
	Camera();
	virtual ~Camera();
	virtual void SetView();
	void SetEye(float x, float y, float z);
	D3DXVECTOR3 GetEye();
	void SetRotation(float x, float y);
	D3DXVECTOR2 GetRotation();
	D3DXMATRIX GetView();
	D3DXVECTOR3 GetForward() { return forward; }
	virtual void Update(float DeltaTime) = 0;
};