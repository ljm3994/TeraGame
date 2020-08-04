#pragma once
#include "FrameWork\Buffer\Model\BoneBuffer.h"
#include <FrameWork\Render\Model\ModelRenderType.h>
#include <FrameWork\Assimp\AssimpType.h>
#include <FrameWork\Assimp\SkinnedMesh.h>

class AnimationControll
{
	vector<BoneAnimation*> Animations;
	AnimationState playAnimationState[MAX_INSTANCECOUNT];
	AnimationState ChangeAnimationState[MAX_INSTANCECOUNT];
	D3DXMATRIX	CurrentBones[MAX_INSTANCECOUNT][BONE_MAX_COUNT];
	D3DXMATRIX	NextBones[MAX_INSTANCECOUNT][BONE_MAX_COUNT];
	D3DXMATRIX	cCurrentBones[MAX_INSTANCECOUNT][BONE_MAX_COUNT];
	D3DXMATRIX	cNextBones[MAX_INSTANCECOUNT][BONE_MAX_COUNT];
	BoneBuffer* bonebuffer;
	ComPtr<ID3D11ShaderResourceView> Currentsrv;
	ComPtr<ID3D11Texture2D> Currenttexture;
	ComPtr<ID3D11ShaderResourceView> Nextsrv;
	ComPtr<ID3D11Texture2D> Nexttexture;
	ComPtr<ID3D11ShaderResourceView> cCurrentsrv;
	ComPtr<ID3D11Texture2D> cCurrenttexture;
	ComPtr<ID3D11ShaderResourceView> cNextsrv;
	ComPtr<ID3D11Texture2D> cNexttexture;
	function<void(bool, int, UINT)> AnimationEvent;
	float AnimationBlendTime[MAX_INSTANCECOUNT];
	float AnimationCurrentBlendTime[MAX_INSTANCECOUNT];
	UINT instanceCount;
public:
	AnimationControll(UINT instanceCount);
	~AnimationControll();
	void SetAnimationEvent(function<void(bool, int, UINT)> Event) { AnimationEvent = Event; }
	void Update(float Time, UINT instanceID, vector<map<string, vector<Transform*>>>& BoneTransformMap, const vector<Bone*>& boneSize);
	void Render();
	bool ChangeAnimation(int Index, UINT instanceID, ANI_STATE State, float BlendTime);
	void ReadAnimationData(string FilePath);
	void SetSrv();
	void CreateTexture();
private:
	void Play(AnimationState* animstate, float DeltaTime, bool isChange, UINT instanceID);
	void BlendAnimation(float DeltaTime, UINT instanceID);
	void ReadAnimation(float DeltaTime, UINT instanceID , vector<map<string, vector<Transform*>>>& BoneTransformMap, const vector<Bone*>& bones);
};