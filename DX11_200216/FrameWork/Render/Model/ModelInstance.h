#pragma once
#include "FrameWork\Buffer\Model\InstanceBuffer.h"
#include "FrameWork\Buffer\Base\ConstantBuffer.h"
#include <FrameWork\DXTypes\DepthStencilBuffer.h>

class BoxCollision;
enum class MODELTYPE
{
	SKINNING_MODELTYPE,
	STATIC_MODELTYPE,
};
class ModelInstance
{
protected:
	Shader* shader;
	map<string, Material*> materials;
	vector<Transform*> Worlds;
	UINT instanceCount;
	D3DXMATRIX * worldmat;
	InstanceBuffer* buffer;
	VertexBuffer* Instancebuffer;
	vector<BoxCollision*> collision;
	bool isCollision;
public:
	ModelInstance(string FileName, string ShaderName, bool isCollision, UINT instanceCount);
	BoxCollision* GetColl(UINT instanceID) { return collision[instanceID]; }
	virtual ~ModelInstance();
	void UpdateTransform();
	bool GetActive(UINT InstanceID);
	void SetActive(UINT InstanceID, bool val);
	void ReductionSetting(DepthStencilBuffer& depthbuffer);
	virtual void Update(float DeltaTime) = 0;
	virtual void Render() = 0;
	virtual void ReadMesh(string FileName) = 0;
	virtual void ReadMaterial(string FileName);
	void CreateReductionTarget(UINT Width, UINT Height);
	Transform* GetTransform(UINT instanceID);
	InstanceBuffer* GetBuffer();
	Material* GetMaterial(string MatName) { return materials[MatName]; }
	Shader* GetShader() { return shader; }
};