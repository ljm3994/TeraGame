#include "Framework.h"
#include "Animator.h"
#include "AssimpHelper.h"
#include <FrameWork\Utility\BinaryHelper.h>

Animator::Animator()

{

}

Animator::~Animator()
{
}
void Animator::ExtractAnimation(const aiScene* scene)
{
	for (UINT i = 0; i < scene->mNumAnimations; i++)
	{
		AnimationClip* clip = new AnimationClip();
		aiAnimation* anim = scene->mAnimations[i];

		clip->Name = anim->mName.C_Str();
		clip->Duration = (float)anim->mDuration;
		clip->TickPerSecond = anim->mTicksPerSecond != 0.0f ? (float)anim->mTicksPerSecond : 25.0f;

		for (UINT j = 0; j < anim->mNumChannels; j++)
		{
			AnimationCurve* curve = new AnimationCurve();
			aiNodeAnim* animnode = anim->mChannels[j];

			curve->Name = animnode->mNodeName.C_Str();

			for (UINT k = 0; k < animnode->mNumScalingKeys; k++)
			{
				CurveVecKey scalekey;
				scalekey.Time = (float)animnode->mScalingKeys[k].mTime;
				scalekey.val.x = (float)animnode->mScalingKeys[k].mValue.x;
				scalekey.val.y = (float)animnode->mScalingKeys[k].mValue.y;
				scalekey.val.z = (float)animnode->mScalingKeys[k].mValue.z;
				curve->ScaleKeys.emplace_back(scalekey);
			}

			for (UINT k = 0; k < animnode->mNumRotationKeys; k++)
			{
				CurveQuternionKey rotkey;
				rotkey.Time = (float)animnode->mRotationKeys[k].mTime;
				rotkey.val.x = (float)animnode->mRotationKeys[k].mValue.x;
				rotkey.val.y = (float)animnode->mRotationKeys[k].mValue.y;
				rotkey.val.z = (float)animnode->mRotationKeys[k].mValue.z;
				rotkey.val.w = (float)animnode->mRotationKeys[k].mValue.w;
				curve->RotationKeys.emplace_back(rotkey);
			}

			for (UINT k = 0; k < animnode->mNumPositionKeys; k++)
			{
				CurveVecKey poskey;
				poskey.Time = (float)animnode->mPositionKeys[k].mTime;
				poskey.val.x = (float)animnode->mPositionKeys[k].mValue.x;
				poskey.val.y = (float)animnode->mPositionKeys[k].mValue.y;
				poskey.val.z = (float)animnode->mPositionKeys[k].mValue.z;
				curve->PositionKeys.emplace_back(poskey);
			}
			clip->AnimationCurves.insert(make_pair(curve->Name, curve));
		}
		Animations.emplace_back(clip);
	}
}

void Animator::SaveAnimationFile(string SaveFile, bool isOverWrite)
{
	if (!isOverWrite)
	{
		if (Helper::IsExistFile(SaveFile))
			return;
	}

	Helper::CreateFolders(SaveFile, true);

	BinaryWrite* w = new BinaryWrite(Helper::stringTowstring(&SaveFile));

	UINT Size = BoneAnimations.size();
	w->Uint(&Size);

	for (auto item : BoneAnimations)
	{
		w->String(&item->Name);
		w->Float(&item->Duration);
		w->Float(&item->TickPerSecond);
		UINT size = item->FinalMat.size();
		w->Uint(&size);
		for (int j = 0; j < item->FinalMat.size(); j++)
		{
			UINT size2 = item->FinalMat[j].size();
			w->Uint(&size2);
			for (int k = 0; k < size2; k++)
			{
				w->Matrix(&item->FinalMat[j][k]);
				w->Matrix(&item->BoneTransList[j][k]);
			}

			UINT size3 = item->EtcBone[j].size();
			w->Uint(&size3);
			for (int k = 0; k < size3; k++)
			{
				w->String(&item->EtcBone[j][k]->Name);
				w->Matrix(&item->EtcBone[j][k]->transform);
			}
		}
		
		delete item;
	}
	
	delete w;
}

void Animator::CreateBoneAnimation(const map<string, UINT>& boneMap, const vector<AssimpBone*>& bones, const vector< AssimpNode*>& nodes, bool isRootLock)
{
	UINT size = BoneAnimations.size();

	for (UINT i = 0; i < Animations.size(); i++)
	{
		BoneAnimations.emplace_back(new BoneAnimation());
		BoneAnimations[size + i]->Duration = Animations[i]->Duration + 1;
		BoneAnimations[size + i]->Name = Animations[i]->Name;
		BoneAnimations[size + i]->TickPerSecond = Animations[i]->TickPerSecond;

		BoneAnimations[size + i]->BoneTransList.resize((UINT)BoneAnimations[i]->Duration);
		BoneAnimations[size + i]->FinalMat.resize((UINT)BoneAnimations[i]->Duration);
		BoneAnimations[size + i]->EtcBone.resize((UINT)BoneAnimations[i]->Duration);
		D3DXMATRIX * matlist = new D3DXMATRIX[nodes.size()];

		for (UINT k = 0; k < BoneAnimations[size + i]->Duration; k++)
		{
			BoneAnimations[size + i]->BoneTransList[k].resize(bones.size());
			BoneAnimations[size + i]->FinalMat[k].resize(bones.size());
			for (UINT j = 0; j < nodes.size(); j++)
			{
				D3DXMATRIX W, animation, parent;
				D3DXMatrixIdentity(&W);
				auto it = Animations[i]->AnimationCurves.find(nodes[j]->Name);

				auto boneit = boneMap.find(nodes[j]->Name);

				if (it != Animations[i]->AnimationCurves.end())
				{
					animation = it->second->Evaluate((float)k, BoneAnimations[size + i]->Duration);
				}
				else
				{
					animation = nodes[j]->Transform;
				}
				
				if (isRootLock)
				{
					if (nodes[j]->Parent == -1)
					{
						float y = animation._42; // 높이값자체는 그대로 유지하는게 좀더 자연스러움
						D3DXMatrixIdentity(&animation);
						animation._42 = y;
					}
				}

				int ParentIndex = nodes[j]->Parent;

				if (ParentIndex < 0)
					D3DXMatrixIdentity(&parent);
				else
					parent = matlist[ParentIndex];

				matlist[j] = animation * parent;

				if (boneit != boneMap.end())
				{
					int boneindex = boneit->second;
					BoneAnimations[size + i]->BoneTransList[k][boneindex] = bones[boneindex]->Offset * matlist[j];
					BoneAnimations[size + i]->FinalMat[k][boneindex] = matlist[j];
				}
				else
				{
					NoMeshBone* etcbone = new NoMeshBone();
					etcbone->Name = nodes[j]->Name;
					etcbone->transform = matlist[j];
					BoneAnimations[size + i]->EtcBone[k].emplace_back(etcbone);
				}
				
			}
		}
		auto it = Animations[i]->AnimationCurves.begin();

		for (; it != Animations[i]->AnimationCurves.end(); it++)
		{
			it->second->PositionKeys.clear();
			it->second->PositionKeys.shrink_to_fit();
			it->second->ScaleKeys.clear();
			it->second->ScaleKeys.shrink_to_fit();
			it->second->RotationKeys.clear();
			it->second->RotationKeys.shrink_to_fit();

			delete it->second;
		}

		Animations[i]->AnimationCurves.clear();
		delete Animations[i];
		delete[] matlist;
	}

	Animations.clear();
	Animations.shrink_to_fit();
}



