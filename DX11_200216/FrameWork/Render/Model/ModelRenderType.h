#pragma once
#include "FrameWork\Assimp\AssimpType.h"


enum class ANI_STATE : int
{
	ANI_LOOP,
	ANI_ONCE,
	ANI_STOP
};


struct MeshBuffer
{
	vector<VertexModelData> vertices;
	VertexBuffer* vertexbuffer;

	vector<UINT> indices;
	IndexBuffer* indexbuffer;

	string MaterialName;

	~MeshBuffer()
	{
		delete vertexbuffer;
		delete indexbuffer;
	}
};

struct AnimationState
{
	bool isPlaying;
	int Index;
	int CurrentFrame;
	int NextFrame;
	float Weight;
	ANI_STATE State;

	AnimationState()
		: isPlaying(false), Index(0), CurrentFrame(0), NextFrame(1), Weight(0.0f), State(ANI_STATE::ANI_STOP)
	{

	}
};

struct Bone
{
	string Name;
	D3DXMATRIX Transform;
	D3DXMATRIX Offset;
};
