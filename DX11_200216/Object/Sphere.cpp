#include "Framework.h"
#include "Sphere.h"


Sphere::Sphere(float Radius, UINT StackCount, UINT SliceCount)
	: radius(Radius), StackCount(StackCount), sliceCount(SliceCount)
{
	shader = Engine->AddShader("Sphere", L"Sphere");

	CreateData();
	CreateTangent();

	vertexbuffer = make_unique<VertexBuffer>(VertexBuffer(vertices.data(), sizeof(VertexTangent), vertices.size()));
	indexbuffer = make_unique<IndexBuffer>(IndexBuffer(indices.data(), indices.size()));
}

Sphere::Sphere(wstring ShaderPath, float Radius, UINT StackCount, UINT SliceCount)
	: radius(Radius), StackCount(StackCount), sliceCount(SliceCount)
{
	shader = Engine->AddShader(Helper::wstringTostring(&ShaderPath), ShaderPath);

	CreateData();
	CreateTangent();

	vertexbuffer = make_unique<VertexBuffer>(VertexBuffer(vertices.data(), sizeof(VertexTangent), vertices.size()));
	indexbuffer = make_unique<IndexBuffer>(IndexBuffer(indices.data(), indices.size()));
}

Sphere::~Sphere()
{
}

void Sphere::Update(float DeltaTime)
{
	UpdateWorld();
}

void Sphere::Render()
{
	SetVS();

	shader->SetRender(vertexbuffer.get(), indexbuffer.get());
	
	Engine->GetContext()->DrawIndexed(indices.size(), 0, 0);
}

void Sphere::CreateData()
{
	float phiStep = D3DX_PI / StackCount;
	float thetaStep = 2.0f * D3DX_PI / sliceCount;

	for (UINT i = 0; i <= StackCount; i++)
	{
		float phi = i * phiStep;

		for (UINT j = 0; j <= sliceCount; j++)
		{
			float theta = j * thetaStep;

			VertexTangent vertex;

			vertex.Normal.x = sin(phi) * cos(theta);
			vertex.Normal.y = cos(phi);
			vertex.Normal.z = sin(phi) * sin(theta);

			vertex.Position = vertex.Normal * radius;

			vertex.UV.x = (float)j / sliceCount;
			vertex.UV.y = (float)i / StackCount;

			vertices.push_back(vertex);
		}
	}

	for (UINT i = 0; i < StackCount; i++)
	{
		for (UINT j = 0; j < sliceCount; j++)
		{
			indices.push_back((sliceCount + 1) * i + j);//0
			indices.push_back((sliceCount + 1) * i + j + 1);//1
			indices.push_back((sliceCount + 1) * (i + 1) + j);//2

			indices.push_back((sliceCount + 1) * (i + 1) + j);//2
			indices.push_back((sliceCount + 1) * i + j + 1);//1
			indices.push_back((sliceCount + 1) * (i + 1) + j + 1);//3
		}
	}
}

void Sphere::CreateTangent()
{
	for (UINT i = 0; i < indices.size() / 3; i++)
	{
		UINT index0 = indices[i * 3 + 0];
		UINT index1 = indices[i * 3 + 1];
		UINT index2 = indices[i * 3 + 2];

		VertexTangent vertex0 = vertices[index0];
		VertexTangent vertex1 = vertices[index1];
		VertexTangent vertex2 = vertices[index2];

		D3DXVECTOR3 p0 = vertex0.Position;
		D3DXVECTOR3 p1 = vertex1.Position;
		D3DXVECTOR3 p2 = vertex2.Position;

		D3DXVECTOR2 uv0 = vertex0.UV;
		D3DXVECTOR2 uv1 = vertex1.UV;
		D3DXVECTOR2 uv2 = vertex2.UV;

		D3DXVECTOR3 e0 = p1 - p0;
		D3DXVECTOR3 e1 = p2 - p0;

		float u0 = uv1.x - uv0.x;
		float u1 = uv2.x - uv0.x;
		float v0 = uv1.y - uv0.y;
		float v1 = uv2.y - uv0.y;

		float d = 1.0f / (u0 * v1 - v0 * u1);

		D3DXVECTOR3 tangent;

		tangent = (v1 * e0 - v0 * e1) * d;

		vertices[index0].Tanjent += tangent;
		vertices[index1].Tanjent += tangent;
		vertices[index2].Tanjent += tangent;
	}

	for (VertexTangent& vertex : vertices)
	{
		D3DXVECTOR3 t = vertex.Tanjent;
		D3DXVECTOR3 n = vertex.Normal;

		D3DXVECTOR3 temp = (t - n * D3DXVec3Dot(&n, &t));
		D3DXVec3Normalize(&temp, &temp);

		vertex.Tanjent = temp;
	}
}
