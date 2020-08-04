#pragma once
#include "FrameWork/Buffer/Transform/WorldBuffer.h"
#define ANGLERADIAN 0.017453f

class Transform
{
private:
	string tag;

protected:
	D3DXVECTOR3 forward;
	D3DXVECTOR3 right;
	D3DXVECTOR3 up;
	D3DXVECTOR3 LocalPosition;
	D3DXQUATERNION rotation;
	D3DXMATRIX RotMat;
	D3DXVECTOR3 scale;
	D3DXVECTOR3 ScalePivot;
	D3DXVECTOR3 RotationPivot;
	D3DXVECTOR3 OffsetPosition;
	D3DXVECTOR3 pivot;
	float ScaleFactor;
	float YawAngle;
	float PitchAngle;
	float RollAngle;
	D3DXMATRIX world;
	D3DXMATRIX* parent;
	bool isUpdate;
	unique_ptr<WorldBuffer> worldBuffer;

	D3DXVECTOR3 WorldPosition;
public:
	Transform(string tag = "Untagged");
	virtual ~Transform();

	void Init();
	void UpdateWorld();
	void SetVS(UINT slot = 0);

	void SetPosition(float x, float y, float z);
	void SetScale(float x, float y, float z);
	void SetRotation(float x, float y, float z, float w);
	void YawRotation();
	void RollRotation();
	void PitchRotation();
	void SetYawAngle(float Angle) { YawAngle = Angle; }
	void SetRollAngle(float Angle) { RollAngle = Angle; }
	void SetPitchAngle(float Angle) { PitchAngle = Angle; }
	void SetForward(D3DXVECTOR3 val) { forward = val; }
	void SetRight(D3DXVECTOR3 val) { right = val; }
	void SetUp(D3DXVECTOR3 val) { up = val; }
	void SetScalePivot(float x, float y, float z);
	void SetScaleFactor(float value);
	void SetRotationPivot(float x, float y, float z);
	void SetOffSetPosition(float x, float y, float z);
	bool Move(float DeltaTime, D3DXVECTOR3 TagetPos);
	void Turn(D3DXVECTOR3 TagetPos);
	bool GetIsUpdate();
	void SetIsUpdate(bool value);
	void SetParent(D3DXMATRIX* val);
	void SetWorld(D3DXMATRIX val);
	float GetScaleFactor() { return ScaleFactor; }
	D3DXMATRIX* GetWorld();
	D3DXVECTOR3 GetPosition();
	D3DXVECTOR3 GetScale();
	D3DXQUATERNION GetRotaion();
	D3DXVECTOR3 GetForward() { return forward; }
	D3DXVECTOR3 GetRight() { return right; }
	D3DXVECTOR3 GetUp() { return up; }
	D3DXVECTOR3 GetRotationPivo() { return RotationPivot; }
	D3DXVECTOR3 GetScalePivot() { return ScalePivot; }
	D3DXVECTOR3 GetOffSetPosition();
	D3DXVECTOR3 GetZAxis() { return forward; }
	float GetYawAngle() { return YawAngle; }
	float GetPitchAngle() { return PitchAngle; }
	float GetRollAngle() { return RollAngle; }
};