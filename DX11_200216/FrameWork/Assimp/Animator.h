#pragma once
#include "AssimpType.h"

class Animator
{
private:
	friend class SkinnedMesh;

	int Index = 0;
	bool isRootLock;
	vector< AnimationClip*> Animations;
	vector<BoneAnimation*> BoneAnimations;
public:
	Animator();
	~Animator();
	void ExtractAnimation(const aiScene* scene);
	void SaveAnimationFile(string SaveFile, bool isOverWrite);
private:
	void CreateBoneAnimation(const map<string, UINT>& boneMap, const vector<AssimpBone*>& bones, const vector< AssimpNode*>& nodes, bool isRootLock);
	vector< AnimationClip*>& GetAnimations() { return Animations; }
};