#pragma once
#include <FrameWork\Render\Shader\Material.h>

class Sphere : public Transform
{
	Shader* shader;

	vector<VertexTangent> vertices;
	vector<UINT> indices;

	unique_ptr<VertexBuffer> vertexbuffer;
	unique_ptr<IndexBuffer> indexbuffer;

	UINT StackCount;
	UINT sliceCount;
	float radius;
public:
	Sphere(float Radius, UINT StackCount = 20, UINT SliceCount = 20);
	Sphere(wstring ShaderPath, float Radius = 1.0f, UINT StackCount = 20, UINT SliceCount = 20);
	~Sphere();

	void Update(float DeltaTime);
	void Render();

	void CreateData();
	void CreateTangent();

	float GetRadius() { return radius; }
	Shader* GetMaterial() { return shader; }
};