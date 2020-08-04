#pragma once
#include "Framework.h"
#define MAX_BONE 200

struct MaterialInfo
{
	string Name;
	string Diffusefile;
	string Specularfile;
	string NormalFile;
	
	D3DXCOLOR Ambient;
	D3DXCOLOR Diffuse;
	D3DXCOLOR Specular;

	MaterialInfo()
	{
		Name = "";
		Diffusefile = "";
		Specularfile = "";
		NormalFile = "";
		Ambient = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);
		Diffuse = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);
		Specular = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);
	}
};

struct BoneWeight
{
	UINT BoneIndex[4];
	float Weights[4];

	BoneWeight()
	{
		ZeroMemory(BoneIndex, sizeof(UINT) * 4);
		ZeroMemory(Weights, sizeof(float) * 4);
	}

	void Add(const UINT& boneindex, const float& weight)
	{
		for (UINT i = 0; i < 4; i++)
		{
			if (Weights[i] == 0.0f)
			{
				BoneIndex[i] = boneindex;
				Weights[i] = weight;
				return;
			}
		}
	}

	void Normalize()
	{
		float Total = 0.0f;
		for (UINT i = 0; i < 4; i++)
		{
			if (Weights[i] != 0.0f)
			{
				Total += Weights[i];
			}
		}
		for (UINT i = 0; i < 4; i++)
		{
			if (Weights[i] != 0.0f)
			{
				Weights[i] /= Total;
			}
		}
	}
};

struct AssimpNode
{
	int Index;
	string Name;
	int Parent;
	D3DXMATRIX Transform;
};

struct AssimpBone
{
	string Name;
	D3DXMATRIX transform;
	D3DXMATRIX Offset;
};

struct Mesh
{
	string Name;
	string MaterialName;
	vector<VertexModelData> vertices;
	vector<UINT> indics;
};

struct CurveVecKey
{
	float Time;
	D3DXVECTOR3 val;
};

struct CurveQuternionKey
{
	float Time;
	D3DXQUATERNION val;
};

struct AnimationCurve
{
	string Name;
	vector<CurveVecKey> PositionKeys;
	vector<CurveQuternionKey> RotationKeys;
	vector<CurveVecKey> ScaleKeys;

	D3DXMATRIX Evaluate(float DeltaTime, int Duration);
	D3DXVECTOR3 CalcInterpolatedPosition(float DeltaTime, int Duration);
	D3DXVECTOR3 CalcInterpolatedScale(float DeltaTime, int Duration);
	D3DXQUATERNION CalcInterpolatedRotation(float DeltaTime, int Duration);
};

struct AnimationClip
{
	string Name;
	float Duration;
	float TickPerSecond;
	map<string, AnimationCurve*> AnimationCurves;
};
struct NoMeshBone
{
	string Name;
	D3DXMATRIX transform;
};
struct BoneAnimation
{
	string Name;
	float Duration;
	float TickPerSecond;
	vector<vector<D3DXMATRIX>> BoneTransList;
	vector<vector<D3DXMATRIX>> FinalMat;
	vector<vector<NoMeshBone*>> EtcBone;
};

