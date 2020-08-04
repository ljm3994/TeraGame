#pragma once

class StaticModel : public ModelInstance
{
	vector<StaticMeshDesc*> meshs;
	D3DXVECTOR3 Min, Max;
public:
	StaticModel(string FileName, string ShaderFile, bool isCollision, UINT instanceCount);
	~StaticModel();

	virtual void Update(float DeltaTime) override;
	virtual void Render() override;
	virtual void ReadMesh(string FileName) override;
	StaticMeshDesc* GetMeshData(int Index) { return meshs[Index]; }
	UINT GetMeshCount() { return meshs.size(); }
};