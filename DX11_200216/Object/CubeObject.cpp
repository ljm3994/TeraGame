#include "Framework.h"
#include "CubeObject.h"

CubeObject::CubeObject()
{
	mat = new Material("Test", L"Test");
	SetVertex();
	SetIndex();
	scale = D3DXVECTOR3(10.0f, 10.0f, 10.0f);
	LocalPosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
}

CubeObject::~CubeObject()
{
	delete index;
	delete buffer;
	delete mat;
}

void CubeObject::SetVertex()
{
	m_Vertex.emplace_back(VertexManager(0, Radius, 0));

	float Step = D3DX_PI / stackCount;
	float theta2 = 2.0f * D3DX_PI / sliceCount;

	for (int i = 1; i <= stackCount - 1; i++)
	{
		float phi = i * Step;

		for (int j = 0; j <= sliceCount; j++)
		{
			float theta = j * theta2;

			D3DXVECTOR3 Position = D3DXVECTOR3(sinf(phi) * cosf(theta), cosf(phi), sinf(phi) * sinf(theta));
			//D3DXVECTOR3 Tan = D3DXVECTOR3(-Radius * sinf(phi) * sinf(theta), 0, Radius * sinf(phi) * cosf(theta));
			//D3DXVECTOR3 BIN = D3DXVECTOR3(0, -Radius * cosf(phi), Radius * sinf(phi) * cosf(theta));
			//D3DXVec3Normalize(&Tan, &Tan);
			Position *= Radius;

			m_Vertex.emplace_back(VertexManager(Position));
			//vertices.emplace_back(VertexTanjent(Position, UV, Normal, Tan, BIN));
		}
	}

	m_Vertex.emplace_back(VertexManager(0, -Radius, 0));

	buffer = new VertexBuffer(m_Vertex.data(), sizeof(VertexManager), m_Vertex.size());
}

void CubeObject::ObjectUpdate()
{
	UpdateWorld();
}

void CubeObject::ObjectRender()
{
	SetVS();
	mat->GetShader()->SetRender(buffer, index);

	Engine->GetContext()->DrawIndexed(indices.size(), 0, 0);
}

void CubeObject::SetIndex()
{
	for (int i = 1; i <= sliceCount; i++)
	{
		indices.emplace_back(0);
		indices.emplace_back(i + 1);
		indices.emplace_back(i);
	}

	int Index = 1;
	int VertexCount = sliceCount + 1;

	for (int i = 0; i < stackCount - 2; i++)
	{
		for (int j = 0; j < sliceCount; j++)
		{
			indices.emplace_back(Index + i * VertexCount + j);
			indices.emplace_back(Index + i * VertexCount + j + 1);
			indices.emplace_back(Index + (i + 1) * VertexCount + j);

			indices.emplace_back(Index + (i + 1) * VertexCount + j);
			indices.emplace_back(Index + i * VertexCount + j + 1);
			indices.emplace_back(Index + (i + 1) * VertexCount + j + 1);
		}
	}
	int Pole = m_Vertex.size() - 1;
	Index = Pole - VertexCount;
	for (int i = 0; i < sliceCount; i++)
	{
		indices.emplace_back(Pole);
		indices.emplace_back(Index + i);
		indices.emplace_back(Index + i + 1);
	}

	this->index = new IndexBuffer(indices.data(), indices.size());
}
