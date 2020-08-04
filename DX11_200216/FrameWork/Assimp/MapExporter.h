#pragma once
#include <FrameWork\Assimp\AssimpType.h>

class MapExporter
{
	struct MeshDesc
	{
		string MatName;
		vector<VertexTangent> vertices;
		vector<UINT> indices;
	};
	D3DXVECTOR3 Max, Min;

	vector<MeshDesc> meshdescs;
	vector<MaterialInfo> materialinfos;
	const aiScene* scene;
public:
	MapExporter(const aiScene* scene);
	~MapExporter();
	void WriteMesh(string SaveFile, bool isOverWrite);
	void WriteMaterial(string SaveFile, bool isOverWrite);
private:
	void ZeroLocalCaluculation();
	void ReadMesh(aiNode* node);
	void ReadMaterial();
	string WriteTexture(const aiScene* scene, string SavePath, string FilePath);
};