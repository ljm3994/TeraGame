#pragma once
#include "FrameWork/Game/Transform/Transform.h"
#include "FrameWork/Render/Shader/Material.h"
class CubeObject : public Transform
{
private:
	VertexBuffer* buffer;
	IndexBuffer* index;
	Material* mat;
	vector<VertexManager> m_Vertex;
	vector <UINT> indices;

	float Radius = 0.5f;
	UINT sliceCount = 20;
	UINT stackCount = 20;
public:
	CubeObject();
	~CubeObject();
	void SetVertex();
	void ObjectUpdate();
	void ObjectRender();
	void SetIndex();
};