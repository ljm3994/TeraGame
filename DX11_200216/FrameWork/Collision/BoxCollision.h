#pragma once
#include "FrameWork\Collision\Collision.h"

struct Obb
{
	Vector3 position;

	Vector3 axisX;
	Vector3 axisY;
	Vector3 axisZ;

	Vector3 halfSize;
};

class BoxCollision : public Collision
{
private:
	Vector3 minBox;
	Vector3 maxBox;

	VertexManager vertex[8];
public:
	BoxCollision(Vector3 min, Vector3 max);
	~BoxCollision();
	// Collision을(를) 통해 상속됨
	virtual bool IsCollision(Collision* collision) override;
	virtual bool IsCollision(Ray ray, float* distance = nullptr, Vector3* contact = nullptr) override;

	Vector3 GetMin();
	Vector3 GetMax();

private:
	void Create();

	bool AABB(BoxCollision* collision);
	bool OBB(BoxCollision* collision);

	Obb GetObb();

	bool SeperateAxis(Vector3 position, Vector3 direction, Obb box1, Obb box2);
};