#pragma once

struct VertexManager
{
	D3DXVECTOR3 Position;

	VertexManager() : Position(0, 0, 0)
	{

	}
	VertexManager(float x, float y, float z) : Position(x, y, z)
	{

	}
	VertexManager(D3DXVECTOR3 Position) : Position(Position)
	{

	}
};

struct VertexUV
{
	D3DXVECTOR3 Position;
	D3DXVECTOR2 UV;

	VertexUV() : Position(0, 0, 0), UV(0, 0)
	{

	}
	VertexUV(float x, float y, float z, float UVx, float UVy)
		: Position(x, y, z), UV(UVx, UVy)
	{

	}
	VertexUV(D3DXVECTOR3 Position, D3DXVECTOR2 Uv)
		: Position(Position), UV(Uv)
	{

	}
};

struct VertexNormal
{
	D3DXVECTOR3 Position;
	D3DXVECTOR3 Normal;
	D3DXVECTOR2 UV;

	VertexNormal() : Position(0, 0, 0), UV(0, 0), Normal(0, 0, 0)
	{

	}
	VertexNormal(float x, float y, float z, float UVx, float UVy, float Normalx, float Normaly, float Normalz)
		: Position(x, y, z), UV(UVx, UVy), Normal(Normalx, Normaly, Normalz)
	{

	}

	VertexNormal(D3DXVECTOR3 Position, D3DXVECTOR2 UV, D3DXVECTOR3 Normal)
		: Position(Position), UV(UV), Normal(Normal)
	{

	}
};


struct VertexTangent
{
	D3DXVECTOR3 Position;
	D3DXVECTOR2 UV;
	D3DXVECTOR3 Normal;
	D3DXVECTOR3 Tanjent;

	VertexTangent() : Position(0, 0, 0), UV(0, 0), Normal(0, 0, 0), Tanjent(0, 0, 0)
	{

	}
	VertexTangent(float x, float y, float z, float UVx, float UVy, float Normalx, float Normaly, float Normalz, float Tanjentx, float Tanjecty, float Tanjectz)
		: Position(x, y, z), UV(UVx, UVy), Normal(Normalx, Normaly, Normalz), Tanjent(Tanjentx, Tanjecty, Tanjectz)
	{

	}
	VertexTangent(D3DXVECTOR3 Position, D3DXVECTOR2 Uv, D3DXVECTOR3 Normal, D3DXVECTOR3 Tan)
		: Position(Position), UV(Uv), Normal(Normal), Tanjent(Tan)
	{

	}
};

struct VertexTangentAlpha
{
	D3DXVECTOR3 Position;
	D3DXVECTOR2 UV;
	D3DXVECTOR3 Normal;
	D3DXVECTOR3 Tanjent;
	D3DXCOLOR Alpha;

	VertexTangentAlpha() : Position(0, 0, 0), UV(0, 0), Normal(0, 0, 0), Tanjent(0, 0, 0), Alpha(0, 0, 0, 0)
	{

	}
	VertexTangentAlpha(float x, float y, float z, float UVx, float UVy, float Normalx, float Normaly, float Normalz, float Tanjentx, float Tanjecty, float Tanjectz, float r, float g, float b, float a)
		: Position(x, y, z), UV(UVx, UVy), Normal(Normalx, Normaly, Normalz), Tanjent(Tanjentx, Tanjecty, Tanjectz), Alpha(r, g, b, a)
	{

	}
	VertexTangentAlpha(D3DXVECTOR3 Position, D3DXVECTOR2 Uv, D3DXVECTOR3 Normal, D3DXVECTOR3 Tan, D3DXCOLOR _Alpha)
		: Position(Position), UV(Uv), Normal(Normal), Tanjent(Tan), Alpha(_Alpha)
	{

	}
};

struct ModelVertex
{
	D3DXVECTOR3 Position;
	D3DXVECTOR2 UV;
	D3DXVECTOR3 Normal;
	D3DXVECTOR3 Tanjent;
	D3DXCOLOR Alpha;
	D3DXVECTOR4 BlendIndex;
	D3DXVECTOR4 BlendWeight;
	UINT BoneIndex;

	ModelVertex() : Position(0, 0, 0), UV(0, 0), Normal(0, 0, 0), Tanjent(0, 0, 0), Alpha(0, 0, 0, 0), BlendIndex(0, 0, 0, 0), BlendWeight(0, 0, 0, 0)
	{
		BoneIndex = 0;
	}
	ModelVertex(float x, float y, float z, float UVx, float UVy, float Normalx, float Normaly, float Normalz, float Tanjentx, float Tanjecty, float Tanjectz, float r, float g, float b, float a)
		: Position(x, y, z), UV(UVx, UVy), Normal(Normalx, Normaly, Normalz), Tanjent(Tanjentx, Tanjecty, Tanjectz), Alpha(r, g, b, a), BlendIndex(0, 0, 0, 0), BlendWeight(0, 0, 0, 0)
	{
		BoneIndex = 0;
	}
	ModelVertex(D3DXVECTOR3 Position, D3DXVECTOR2 Uv, D3DXVECTOR3 Normal, D3DXVECTOR3 Tan, D3DXCOLOR _Alpha)
		: Position(Position), UV(Uv), Normal(Normal), Tanjent(Tan), Alpha(_Alpha), BlendIndex(0, 0, 0, 0), BlendWeight(0, 0, 0, 0)
	{
		BoneIndex = 0;
	}
};

struct VertexParticle
{
	D3DXVECTOR3 Position;
	D3DXVECTOR3 Velocity;
	D3DXVECTOR2 Size;
	float Age;
	UINT ParticleType;

	VertexParticle() : Position(0, 0, 0), Size(0, 0), Velocity(0, 0, 0), Age(0), ParticleType(0)
	{

	}
	VertexParticle(float x, float y, float z, float Velocityx, float Velocityy, float Velocityz, float Width, float Height, float Age, UINT Type)
		: Position(x, y, z), Velocity(Velocityx, Velocityy, Velocityz), Size(Width, Height), Age(Age), ParticleType(Type)
	{

	}
	VertexParticle(D3DXVECTOR3 Position, D3DXVECTOR3 Velocity, D3DXVECTOR2 Size, float Age, UINT Type)
		: Position(Position), Velocity(Velocity), Size(Size), Age(Age), ParticleType(Type)
	{

	}
};

struct VertexParticle2
{
	float age;
	float scale;
	D3DXVECTOR4 Position;
	D3DXCOLOR Color;
	D3DXVECTOR4 Velocity;
	UINT ParticleType;

	VertexParticle2() : Position(0, 0, 0, 1), scale(0), Velocity(0, 0, 0, 1), age(0), ParticleType(0), Color(0, 0, 0, 1)
	{

	}
};
static D3DXVECTOR3 GetNormalVec(D3DXVECTOR3 v1, D3DXVECTOR3 v2, D3DXVECTOR3 v3)
{
	D3DXVECTOR3 A = v2 - v1;
	D3DXVECTOR3 B = v3 - v1;

	D3DXVECTOR3 normal;

	D3DXVec3Cross(&normal, &A, &B);

	return normal;
}

struct VertexModelData
{
	D3DXVECTOR3 Position;
	D3DXVECTOR2 Uv;
	D3DXVECTOR3 Normal;
	D3DXVECTOR3 Tangent;
	D3DXVECTOR4 Indices;
	D3DXVECTOR4 Weights;

	VertexModelData()
		: Position(0, 0, 0), Uv(0, 0), Normal(0, 0, 0), Tangent(0, 0, 0), Indices(0, 0, 0, 0), Weights(0, 0, 0, 0)
	{

	}
};

struct VertexColor
{
	D3DXVECTOR3 Position;
	D3DXCOLOR Color;

	VertexColor(float x, float y, float z, float r, float g, float b, float a)
		: Position(x, y, z), Color(r, g, b, a)
	{

	}
	VertexColor()
		: Position(0, 0, 0), Color(0, 0, 0, 1.0f)
	{

	}
};

