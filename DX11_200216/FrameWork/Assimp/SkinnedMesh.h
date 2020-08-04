#pragma once
#include <FrameWork\Render\Shader\Material.h>
#include <FrameWork\Game\Transform\Transform.h>
#include <FrameWork\Assimp\Animator.h>

class SkinnedMesh
{
	friend class AssimpFactory;

	Animator* animation;

	string FileName;
	const aiScene* scene;

	vector<Mesh*> meshes;
	
	vector<AssimpNode*> nodes;
	vector<AssimpBone*> bones;

	map<string, MaterialInfo*> Materials;
	map<string, UINT> boneMap;

	// 추후 충돌 박스를 만들기 위한 처리......
	D3DXVECTOR3 Min;
	D3DXVECTOR3 Max;

	SkinnedMesh(const aiScene* scene, wstring FileName);
public:
	~SkinnedMesh();
	void ReadAnimation(const aiScene* scene, bool isRootLock);
	vector<Mesh*>& GetMeshData() { return meshes; }
	D3DXMATRIX GetFindBoneWorldMatrix(string Name);
	void WriteMaterialFile(string SaveFile, bool isOverWrite = false);
	void WriteMeshFile(string SaveFile, bool isOverWrite = false);
	void WriteAnimationFile(string SaveFile, bool isOverWrite = false);
private:
	void ReadMatarial();
	void ReadNodeData(aiNode* node, int index, int parent);
	void ReadMeshData(aiNode* node);
	void ReadBoneData(aiMesh* mesh, vector<BoneWeight>& vertex_weights);
	string WriteTexture(const aiScene* scene, string SavePath, string FilePath);
};