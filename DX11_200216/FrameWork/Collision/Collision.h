#pragma once
#include "FrameWork\Buffer\Model\TrailColorBuffer.h"

class Collision : public Transform
{
protected:
	Shader* shader;

	VertexBuffer* vertexbuffer;
	IndexBuffer* indexbuffer;
	TrailColorBuffer* color;
	UINT indexCount;
public:
	Collision();
	~Collision();

	virtual bool IsCollision(Collision* collision) = 0;
	virtual bool IsCollision(Ray ray, float* distance = nullptr, Vector3* contact = nullptr) = 0;
	TrailColorBuffer* GetColor() { return color; }
	virtual void Update(float DeltaTime);
	void Render();
};