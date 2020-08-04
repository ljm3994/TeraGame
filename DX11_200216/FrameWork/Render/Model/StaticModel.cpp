#include "Framework.h"
#include "StaticModel.h"
#include <FrameWork\Utility\BinaryHelper.h>
#include "FrameWork\Render\Shader\Material.h"
#include "FrameWork\Collision\BoxCollision.h"

StaticModel::StaticModel(string FileName, string ShaderFile, bool isCollision, UINT instanceCount)
	: ModelInstance(FileName, ShaderFile, isCollision, instanceCount)
{
	ReadMesh(DefaultMeshPath + FileName + ".mesh");
}

StaticModel::~StaticModel()
{
}

void StaticModel::Update(float DeltaTime)
{
	UpdateTransform();

	if (isCollision)
	{
		for (UINT i = 0; i < instanceCount; i++)
		{
			collision[i]->Update(DeltaTime);
		}
	}
}

void StaticModel::Render()
{
	if (isCollision)
	{
		for (UINT i = 0; i < instanceCount; i++)
		{
			collision[i]->Render();
		}
	}

	Instancebuffer->VSSet(1);
	buffer->SetVSBuffer(11);

	for (UINT i = 0; i < meshs.size(); i++)
	{
		shader->SetRender(meshs[i]->vertexbuffers.get(), meshs[i]->indexbuffers.get());

		materials[meshs[i]->MatName]->Set();

		Engine->GetContext()->DrawIndexedInstanced(meshs[i]->indices.size(), instanceCount,0, 0, 0);
	}
}

void StaticModel::ReadMesh(string FileName)
{
	BinaryReader* r = new BinaryReader(Helper::stringTowstring(&FileName));

	Min.x = r->Float();
	Min.y = r->Float();
	Min.z = r->Float();

	Max.x = r->Float();
	Max.y = r->Float();
	Max.z = r->Float();

	if (isCollision)
	{
		for (UINT i = 0; i < instanceCount; i++)
		{
			BoxCollision* coll = new BoxCollision(Min, Max);
			collision.emplace_back(coll);
			coll->SetParent(Worlds[i]->GetWorld());
		}
	}
	UINT count = r->Uint();
	meshs.resize(count);
	for (UINT i = 0; i < count; i++)
	{
		meshs[i] = new StaticMeshDesc();
		meshs[i]->MatName = r->String();

		UINT verticeCount = r->Uint();
		meshs[i]->vertices.resize(verticeCount);
		void* p = (void*)&meshs[i]->vertices[0];
		UINT ArraySize = sizeof(VertexTangent) * verticeCount;
		r->Byte(&p, &ArraySize);

		UINT indicesCount = r->Uint();
		meshs[i]->indices.resize(indicesCount);
		void* p2 = (void*)&meshs[i]->indices[0];
		UINT ArraySize2 = sizeof(UINT) * indicesCount;
		r->Byte(&p2, &ArraySize2);

		meshs[i]->vertexbuffers.reset(new VertexBuffer(meshs[i]->vertices.data(), sizeof(VertexTangent), meshs[i]->vertices.size()));
		meshs[i]->indexbuffers.reset(new IndexBuffer(meshs[i]->indices.data(), meshs[i]->indices.size()));
	}

	delete r;
}
