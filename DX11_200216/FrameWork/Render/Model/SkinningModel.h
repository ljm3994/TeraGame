#pragma once
#include "AnimationControll.h"
#include "FrameWork\Render\Model\ModelRenderType.h"

class SkinningModel : public ModelInstance
{
private:

	vector<MeshBuffer*> meshbuffer;
	vector<Bone*> bones;
	vector<AssimpNode*> nodes;
	vector<map<string, vector<Transform*>>> BoneTransformMap;
	AnimationControll* animation;
	UINT instanceCount;
public:
	SkinningModel(UINT instanceCount, string FileName, string ShaderName, bool isCollision);
	~SkinningModel();
	void SetAnimationEvent(function<void(bool, int, UINT)> AnimationEvent);
	void AddBoneTransform(string BoneName, UINT instanceID , Transform* transform);
	void AllClearBoneTransform(UINT instancID);
	void RemoveBoneTransform(string BoneName, UINT instanceID);
	void AddAnimation(string FileName);
	bool Play(int Index, UINT instanceIndex, ANI_STATE state, float BlendTime = 0.2f);
	UINT GetInstanceCount() { return instanceCount; }
	virtual void Update(float timeDelta) override;
	virtual void Render() override;
	virtual void ReadMesh(string FileName) override;
};