#include "Framework.h"
#include "AnimationControll.h"
#include <FrameWork\Assimp\AssimpFactory.h>
#include "FrameWork\Utility\BinaryHelper.h"

AnimationControll::AnimationControll(UINT instanceCount)
	:instanceCount(instanceCount)
{
	bonebuffer = new BoneBuffer();
	for (UINT i = 0; i < MAX_INSTANCECOUNT; i++)
	{
		for (UINT j = 0; j < MAX_INSTANCECOUNT; j++)
		{
			D3DXMatrixIdentity(&CurrentBones[i][j]);
			D3DXMatrixIdentity(&NextBones[i][j]);
			D3DXMatrixIdentity(&cCurrentBones[i][j]);
			D3DXMatrixIdentity(&cNextBones[i][j]);
		}
	}
	AnimationEvent = 0;
}

AnimationControll::~AnimationControll()
{
	for (auto item : Animations)
		delete item;

	delete bonebuffer;
}


void AnimationControll::Update(float Time,UINT instanceID, vector<map<string, vector<Transform*>>>& BoneTransformMap, const vector<Bone*>& bone)
{
	if (bonebuffer->GetBondData()->bonebuffer[instanceID].HasAnimation != 0.0f)
	{
		Play(&playAnimationState[instanceID], Time, false, instanceID);
		ReadAnimation(Time, instanceID,BoneTransformMap, bone);
		if (bonebuffer->GetBondData()->bonebuffer[instanceID].isChanging != 0.0f)
		{
			Play(&ChangeAnimationState[instanceID], Time, true, instanceID);
		}
	}
}

void AnimationControll::Render()
{
	Engine->GetContext()->VSSetShaderResources(10, 1, Currentsrv.GetAddressOf());
	Engine->GetContext()->VSSetShaderResources(11, 1, Nextsrv.GetAddressOf());
	Engine->GetContext()->VSSetShaderResources(12, 1, cCurrentsrv.GetAddressOf());
	Engine->GetContext()->VSSetShaderResources(13, 1, cNextsrv.GetAddressOf());

	bonebuffer->SetVSBuffer(10);
}

bool AnimationControll::ChangeAnimation(int Index, UINT instanceID,ANI_STATE State, float BlendTime)
{
	if (bonebuffer->GetBondData()->bonebuffer[instanceID].isChanging)
		return false;

	bonebuffer->GetBondData()->bonebuffer[instanceID].isChanging = true;
	playAnimationState[instanceID].State = ANI_STATE::ANI_STOP;

	ChangeAnimationState[instanceID].isPlaying = true;
	ChangeAnimationState[instanceID].Index = Index;
	ChangeAnimationState[instanceID].Weight = 0.0f;
	ChangeAnimationState[instanceID].CurrentFrame = 0;
	ChangeAnimationState[instanceID].NextFrame = 1;
	ChangeAnimationState[instanceID].State = State;

	AnimationBlendTime[instanceID] = BlendTime;

	return true;
}

void AnimationControll::ReadAnimationData(string FilePath)
{
	BinaryReader* r = new BinaryReader(Helper::stringTowstring(&FilePath));
	UINT AnimationSize = Animations.size();
	UINT Size = r->Uint();

	for (UINT i = 0; i < Size; i++)
	{
		Animations.emplace_back(new BoneAnimation());
		Animations[AnimationSize + i]->Name = r->String();
		Animations[AnimationSize + i]->Duration = r->Float();
		Animations[AnimationSize + i]->TickPerSecond = r->Float();
		UINT TransSize = r->Uint();
		Animations[AnimationSize + i]->BoneTransList.resize(TransSize);
		Animations[AnimationSize + i]->FinalMat.resize(TransSize);
		Animations[AnimationSize + i]->EtcBone.resize(TransSize);
		for (UINT j = 0; j < TransSize; j++)
		{
			UINT tempSizeCount = r->Uint();
			Animations[AnimationSize + i]->BoneTransList[j].resize(tempSizeCount);
			Animations[AnimationSize + i]->FinalMat[j].resize(tempSizeCount);

			for (UINT t = 0; t < tempSizeCount; t++)
			{
				Animations[AnimationSize + i]->FinalMat[j][t] = r->Matrix();
				Animations[AnimationSize + i]->BoneTransList[j][t] = r->Matrix();
			}

			UINT Tempsize2 = r->Uint();
			Animations[AnimationSize + i]->EtcBone[j].resize(Tempsize2);

			for (UINT t = 0; t < Tempsize2; t++)
			{
				Animations[AnimationSize + i]->EtcBone[j][t] = new NoMeshBone();
				Animations[AnimationSize + i]->EtcBone[j][t]->Name = r->String();
				Animations[AnimationSize + i]->EtcBone[j][t]->transform = r->Matrix();
			}

		}
	}

	for (UINT i = 0; i < MAX_INSTANCECOUNT; i++)
	{
		bonebuffer->GetBondData()->bonebuffer[i].HasAnimation = 1.0f;
	}
	delete r;
}

void AnimationControll::SetSrv()
{
	if (!Currenttexture)
	{
		CreateTexture();
		return;
	}

	{
		D3D11_MAPPED_SUBRESOURCE subResource;
		Engine->GetContext()->Map(Currenttexture.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
		memcpy(subResource.pData, CurrentBones,
			BONE_MAX_COUNT * instanceCount * sizeof(D3DXMATRIX));
		Engine->GetContext()->Unmap(Currenttexture.Get(), 0);
	}

	{
		D3D11_MAPPED_SUBRESOURCE subResource;
		Engine->GetContext()->Map(Nexttexture.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
		memcpy(subResource.pData, NextBones,
			BONE_MAX_COUNT * instanceCount * sizeof(D3DXMATRIX));
		Engine->GetContext()->Unmap(Nexttexture.Get(), 0);
	}

	{
		D3D11_MAPPED_SUBRESOURCE subResource;
		Engine->GetContext()->Map(cCurrenttexture.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
		memcpy(subResource.pData, cCurrentBones,
			BONE_MAX_COUNT * instanceCount * sizeof(D3DXMATRIX));
		Engine->GetContext()->Unmap(cCurrenttexture.Get(), 0);
	}

	{
		D3D11_MAPPED_SUBRESOURCE subResource;
		Engine->GetContext()->Map(cNexttexture.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
		memcpy(subResource.pData, cNextBones,
			BONE_MAX_COUNT * instanceCount * sizeof(D3DXMATRIX));
		Engine->GetContext()->Unmap(cNexttexture.Get(), 0);
	}
}

void AnimationControll::CreateTexture()
{
	{
		D3D11_TEXTURE2D_DESC desc = {};
		desc.Width = BONE_MAX_COUNT * 4;
		desc.Height = MAX_INSTANCECOUNT;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.MipLevels = 1;
		desc.SampleDesc.Count = 1;

		D3D11_SUBRESOURCE_DATA subResource;
		subResource.pSysMem = CurrentBones;
		subResource.SysMemPitch = BONE_MAX_COUNT * sizeof(D3DXMATRIX);
		subResource.SysMemSlicePitch = BONE_MAX_COUNT * sizeof(D3DXMATRIX) * MAX_INSTANCECOUNT;

		Engine->GetDevice()->CreateTexture2D(&desc, &subResource, Currenttexture.GetAddressOf());
	}

	{
		D3D11_TEXTURE2D_DESC desc = {};
		desc.Width = BONE_MAX_COUNT * 4;
		desc.Height = MAX_INSTANCECOUNT;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.MipLevels = 1;
		desc.SampleDesc.Count = 1;

		D3D11_SUBRESOURCE_DATA subResource;
		subResource.pSysMem = NextBones;
		subResource.SysMemPitch = BONE_MAX_COUNT * sizeof(D3DXMATRIX);
		subResource.SysMemSlicePitch = BONE_MAX_COUNT * sizeof(D3DXMATRIX) * MAX_INSTANCECOUNT;

		Engine->GetDevice()->CreateTexture2D(&desc, &subResource, Nexttexture.GetAddressOf());
	}

	{
		D3D11_TEXTURE2D_DESC desc = {};
		desc.Width = BONE_MAX_COUNT * 4;
		desc.Height = MAX_INSTANCECOUNT;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.MipLevels = 1;
		desc.SampleDesc.Count = 1;

		D3D11_SUBRESOURCE_DATA subResource;
		subResource.pSysMem = cCurrentBones;
		subResource.SysMemPitch = BONE_MAX_COUNT * sizeof(D3DXMATRIX);
		subResource.SysMemSlicePitch = BONE_MAX_COUNT * sizeof(D3DXMATRIX) * MAX_INSTANCECOUNT;

		Engine->GetDevice()->CreateTexture2D(&desc, &subResource, cCurrenttexture.GetAddressOf());
	}

	{
		D3D11_TEXTURE2D_DESC desc = {};
		desc.Width = BONE_MAX_COUNT * 4;
		desc.Height = MAX_INSTANCECOUNT;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.MipLevels = 1;
		desc.SampleDesc.Count = 1;

		D3D11_SUBRESOURCE_DATA subResource;
		subResource.pSysMem = cNextBones;
		subResource.SysMemPitch = BONE_MAX_COUNT * sizeof(D3DXMATRIX);
		subResource.SysMemSlicePitch = BONE_MAX_COUNT * sizeof(D3DXMATRIX) * MAX_INSTANCECOUNT;
		HRESULT hr;
		hr = Engine->GetDevice()->CreateTexture2D(&desc, &subResource, cNexttexture.GetAddressOf());
		DEBUG(hr);
	}

	{
		D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
		desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		desc.Texture2DArray.MipLevels = 1;
		desc.Texture2DArray.ArraySize = 1;

		Engine->GetDevice()->CreateShaderResourceView(Currenttexture.Get(), &desc, Currentsrv.GetAddressOf());
	}

	{
		D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
		desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		desc.Texture2DArray.MipLevels = 1;
		desc.Texture2DArray.ArraySize = 1;

		Engine->GetDevice()->CreateShaderResourceView(Nexttexture.Get(), &desc, Nextsrv.GetAddressOf());
	}

	{
		D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
		desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		desc.Texture2DArray.MipLevels = 1;
		desc.Texture2DArray.ArraySize = 1;

		Engine->GetDevice()->CreateShaderResourceView(cCurrenttexture.Get(), &desc, cCurrentsrv.GetAddressOf());
	}

	{
		D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
		desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		desc.Texture2DArray.MipLevels = 1;
		desc.Texture2DArray.ArraySize = 1;

		Engine->GetDevice()->CreateShaderResourceView(cNexttexture.Get(), &desc, cNextsrv.GetAddressOf());
	}
}

void AnimationControll::Play(AnimationState* animstate, float DeltaTime, bool isChange, UINT instanceID)
{
	switch (animstate->State)
	{
	case ANI_STATE::ANI_ONCE:
		if (!animstate->isPlaying)
			return;

		animstate->Weight += DeltaTime * Animations[animstate->Index]->TickPerSecond;
		if (animstate->Weight > 1.0f)
		{
			animstate->Weight = 0.0f;
			animstate->CurrentFrame++;
			animstate->NextFrame++;

			if (animstate->NextFrame == Animations[animstate->Index]->Duration)
			{
				if (AnimationEvent)
				{
					AnimationEvent(true, animstate->Index, instanceID);
				}

				animstate->isPlaying = false;
				animstate->CurrentFrame = 0;
				animstate->NextFrame = 1;
			}
		}
		break;
	case ANI_STATE::ANI_LOOP:
		animstate->Weight += DeltaTime * Animations[animstate->Index]->TickPerSecond;

		if (animstate->Weight > 1.0f)
		{
			animstate->Weight = 0.0f;
			animstate->CurrentFrame++;
			animstate->NextFrame++;
			if (animstate->NextFrame == Animations[animstate->Index]->Duration)
			{
				if (AnimationEvent)
				{
					AnimationEvent(true, animstate->Index, instanceID);
				}

				animstate->CurrentFrame = 0;
				animstate->NextFrame = 1;
			}
		}
		break;
	}
}

void AnimationControll::BlendAnimation(float DeltaTime, UINT instanceID)
{
	AnimationCurrentBlendTime[instanceID] += DeltaTime;

	bonebuffer->GetBondData()->bonebuffer[instanceID].ChangingWeight += DeltaTime / AnimationBlendTime[instanceID];

	if (AnimationCurrentBlendTime >= AnimationBlendTime)
	{
		AnimationCurrentBlendTime[instanceID] = 0.0f;
		playAnimationState[instanceID].CurrentFrame = ChangeAnimationState[instanceID].CurrentFrame;
		playAnimationState[instanceID].Index = ChangeAnimationState[instanceID].Index;
		playAnimationState[instanceID].isPlaying = ChangeAnimationState[instanceID].isPlaying;
		playAnimationState[instanceID].NextFrame = ChangeAnimationState[instanceID].NextFrame;
		playAnimationState[instanceID].State = ChangeAnimationState[instanceID].State;
		bonebuffer->GetBondData()->bonebuffer[instanceID].isChanging = false;
		bonebuffer->GetBondData()->bonebuffer[instanceID].ChangingWeight = 0.0f;
	}
}

void AnimationControll::ReadAnimation(float DeltaTime, UINT instanceID, vector<map<string, vector<Transform*>>>& BoneTransformMap, const vector<Bone*>& bones)
{
	if (bonebuffer->GetBondData()->bonebuffer[instanceID].isChanging)
	{
		BlendAnimation(DeltaTime, instanceID);
	}
	for (UINT i = 0; i < bones.size(); i++)
	{
		CurrentBones[instanceID][i] = Animations[playAnimationState[instanceID].Index]->BoneTransList[playAnimationState[instanceID].CurrentFrame][i];
		NextBones[instanceID][i] = Animations[playAnimationState[instanceID].Index]->BoneTransList[playAnimationState[instanceID].NextFrame][i];
		if (BoneTransformMap[instanceID].count(bones[i]->Name) > 0)
		{
			for (UINT j = 0; j < BoneTransformMap[instanceID][bones[i]->Name].size(); j++)
			{
				BoneTransformMap[instanceID][bones[i]->Name][j]->SetWorld(Animations[playAnimationState[instanceID].Index]->FinalMat[playAnimationState[instanceID].CurrentFrame][i]);
			}
		}
	}

	for (UINT i = 0; i < Animations[playAnimationState[instanceID].Index]->EtcBone[playAnimationState[instanceID].CurrentFrame].size(); i++)
	{
		if (BoneTransformMap[instanceID].count(Animations[playAnimationState[instanceID].Index]->EtcBone[playAnimationState[instanceID].CurrentFrame][i]->Name) > 0)
		{
			for (UINT j = 0; j < BoneTransformMap[instanceID][Animations[playAnimationState[instanceID].Index]->EtcBone[playAnimationState[instanceID].CurrentFrame][i]->Name].size(); j++)
			{
				BoneTransformMap[instanceID][Animations[playAnimationState[instanceID].Index]->EtcBone[playAnimationState[instanceID].CurrentFrame][i]->Name][j]->SetWorld(Animations[playAnimationState[instanceID].Index]->EtcBone[playAnimationState[instanceID].CurrentFrame][i]->transform);
			}
		}
	}
	bonebuffer->GetBondData()->bonebuffer[instanceID].Bonesweight = playAnimationState[instanceID].Weight;

	if (bonebuffer->GetBondData()->bonebuffer[instanceID].isChanging)
	{
		for (UINT i = 0; i < bones.size(); i++)
		{
			CurrentBones[instanceID][i] =
				Animations[ChangeAnimationState[instanceID].Index]->BoneTransList[ChangeAnimationState[instanceID].CurrentFrame][i];
			NextBones[instanceID][i] =
				Animations[ChangeAnimationState[instanceID].Index]->BoneTransList[ChangeAnimationState[instanceID].NextFrame][i];
		}

		bonebuffer->GetBondData()->ChangeBuffer[instanceID].Bonesweight = ChangeAnimationState[instanceID].Weight;
	}
}
