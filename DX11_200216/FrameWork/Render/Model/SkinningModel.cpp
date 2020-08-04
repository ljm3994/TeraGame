#include "Framework.h"
#include "SkinningModel.h"
#include "FrameWork\Utility\Xml.h"
#include "FrameWork\Utility\BinaryHelper.h"
#include "FrameWork\Collision\BoxCollision.h"

SkinningModel::SkinningModel(UINT instanceCount, string FileName, string ShaderName, bool isCollision)
	: ModelInstance(FileName, ShaderName, isCollision, instanceCount), animation(nullptr), instanceCount(instanceCount)
{
	ReadMesh(DefaultMeshPath + FileName + ".mesh");
	BoneTransformMap.resize(instanceCount);
}

SkinningModel::~SkinningModel()
{
	if(animation)
		delete animation;

	for (auto item : meshbuffer)
		delete item;

	for (auto item : bones)
		delete item;
}

void SkinningModel::SetAnimationEvent(function<void(bool, int, UINT)> AnimationEvent)
{
	animation->SetAnimationEvent(AnimationEvent);
}

void SkinningModel::AddBoneTransform(string BoneName, UINT instanceID, Transform* transform)
{
	if (BoneTransformMap[instanceID].count(BoneName) == 0)
	{
		for (auto item : bones)
		{
			if (item->Name == BoneName)
			{
				transform->SetWorld(item->Transform);
				break;
			}
		}

		BoneTransformMap[instanceID][BoneName].emplace_back(transform);
	}
	else
	{
		BoneTransformMap[instanceID][BoneName].emplace_back(transform);
	}
}

void SkinningModel::AllClearBoneTransform(UINT instancID)
{
	BoneTransformMap[instancID].clear();
}

void SkinningModel::RemoveBoneTransform(string BoneName, UINT instanceID)
{
	if (BoneTransformMap[instanceID].count(BoneName) > 0)
	{
		BoneTransformMap[instanceID].erase(BoneName);
	}
}


void SkinningModel::AddAnimation(string FileName)
{
	if (!animation)
		animation = new AnimationControll(instanceCount);

	animation->ReadAnimationData(AnimationDefaultPath + FileName + ".anim");
}

bool SkinningModel::Play(int Index, UINT instanceIndex, ANI_STATE state, float BlendTime)
{
	if (animation)
		return animation->ChangeAnimation(Index, instanceIndex, state, BlendTime);

	return false;
}

void SkinningModel::Update(float timeDelta)
{
	UpdateTransform();

	if (animation)
	{
		for (UINT i = 0; i < instanceCount; i++)
		{
			if (buffer->GetIsActive(i))
			{
				animation->Update(timeDelta, i, BoneTransformMap, bones);

				if (isCollision)
				{
					collision[i]->Update(timeDelta);
				}
			}
		}

		animation->SetSrv();
	}
}

void SkinningModel::Render()
{
	Instancebuffer->VSSet(1);
	buffer->SetVSBuffer(11);

	if (animation)
		animation->Render();

	for (UINT i = 0; i < meshbuffer.size(); i++)
	{
		if (materials.count(meshbuffer[i]->MaterialName) > 0)
		{
			Material* mat = materials[meshbuffer[i]->MaterialName];

			mat->Set();
		}

		shader->SetRender(meshbuffer[i]->vertexbuffer, meshbuffer[i]->indexbuffer);
		Engine->GetContext()->DrawIndexedInstanced(meshbuffer[i]->indices.size(), instanceCount, 0, 0, 0);

		if (materials.count(meshbuffer[i]->MaterialName) > 0)
		{
			materials[meshbuffer[i]->MaterialName]->Clear();
		}
	}

	if (isCollision)
	{
		for (UINT i = 0; i < instanceCount; i++)
		{
			collision[i]->Render();
		}
	}
}

void SkinningModel::ReadMesh(string FileName)
{
	BinaryReader* r = new BinaryReader(Helper::stringTowstring(&FileName));

	D3DXVECTOR3 Min;
	Min.x = r->Float();
	Min.y = r->Float();
	Min.z = r->Float();

	D3DXVECTOR3 Max;
	Max.x = r->Float();	
	Max.y = r->Float();
	Max.z = r->Float();

	for (UINT i = 0; i < instanceCount; i++)
	{
		BoxCollision* coll = new BoxCollision(Min, Max);
		collision.emplace_back(coll);
		coll->SetParent(Worlds[i]->GetWorld());
		coll->SetPitchAngle(90.0f * D3DX_PI / 180.0f);
		coll->UpdateWorld();
		coll->SetPitchAngle(0.0f);
	}

	UINT count = r->Uint();
	bones.resize(count);
	for (UINT i = 0; i < count; i++)
	{
		bones[i] = new Bone();
		bones[i]->Name = r->String();
		bones[i]->Offset = r->Matrix();
		bones[i]->Transform = r->Matrix();
	}

	count = r->Uint();
	nodes.resize(count);
	for (UINT i = 0; i < count; i++)
	{
		nodes[i] = new AssimpNode();
		nodes[i]->Name = r->String();
		nodes[i]->Index = r->Int();
		nodes[i]->Parent = r->Int();
		nodes[i]->Transform = r->Matrix();
	}

	count = r->Uint();
	meshbuffer.resize(count);
	for (UINT i = 0; i < count; i++)
	{
		meshbuffer[i] = new MeshBuffer();
		meshbuffer[i]->MaterialName = r->String();

		UINT verticeCount = r->Uint();
		meshbuffer[i]->vertices.resize(verticeCount);
		void* p = (void*)&meshbuffer[i]->vertices[0];
		UINT ArraySize = sizeof(VertexModelData) * verticeCount;
		r->Byte(&p, &ArraySize);

		UINT indicesCount = r->Uint();
		meshbuffer[i]->indices.resize(indicesCount);
		void* p2 = (void*)&meshbuffer[i]->indices[0];
		UINT ArraySize2 = sizeof(UINT) * indicesCount;
		r->Byte(&p2, &ArraySize2);

		meshbuffer[i]->vertexbuffer = new VertexBuffer(meshbuffer[i]->vertices.data(), sizeof(VertexModelData), meshbuffer[i]->vertices.size());
		meshbuffer[i]->indexbuffer = new IndexBuffer(meshbuffer[i]->indices.data(), meshbuffer[i]->indices.size());
	}

	delete r;
}

