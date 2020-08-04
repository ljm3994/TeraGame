#pragma once
#include <FrameWork\Render\Shader\Material.h>
#include "FrameWork\Buffer\Compute\ComputeDataBuffer.h"
#include "FrameWork\Buffer\Compute\RayBuffer.h"

class TerrainCell : public Transform
{
private:
	struct InuputDesc
	{
		UINT index;
		D3DXVECTOR3 v0, v1, v2;

		InuputDesc() : index(0), v0(0, 0, 0), v1(0, 0, 0), v2(0, 0, 0) {}
	};

	struct OutputDesc
	{
		UINT picked;
		float u, v, distance;

		OutputDesc() : picked(0), u(0.0f), v(0.0f), distance(0.0f) {}
	};

	Shader* lineShader;
	Shader* PickShader;
	InuputDesc* input;
	OutputDesc* output;
	ComputeDataBuffer* computebuffer;
	RayBuffer* raybuffer;

	UINT size;
	D3DXVECTOR3 CenterPosition;
	vector<D3DXVECTOR3> vertexList;
	Material* material;
	StaticMeshDesc* terrainmesh;
	vector<VertexColor> linevertices;
	vector<UINT> lineindices;
	unique_ptr<VertexBuffer> vertexbuffers;
	unique_ptr<IndexBuffer> indexbuffers;
	unique_ptr<VertexBuffer> linevertexbuffers;
	unique_ptr<IndexBuffer> lineindexbuffers;

	float MaxWidth;
	float MaxHeight;
	float MaxDepth;
	float MinWidth;
	float MinHeight;
	float MinDepth;
public:
	TerrainCell();
	~TerrainCell();
	void ReadMapData(StaticMeshDesc* desc, Material* mat);
	void CalculateCellDimensions();
	void CreateLine(D3DXCOLOR LineColor, D3DXVECTOR2 Width, D3DXVECTOR2 Height, D3DXVECTOR2 Depth);
	void Update();
	void Render(Shader* shader);
	void GetMinMaxWidth(float& MinWidth, float& MaxWidth);
	void GetMinMaxHeight(float& MinHeight, float& MaxHeight);
	void GetMinMaxDepth(float& MinDepth, float& MaxDepth);
	void RenderLine();
	void CreateTile();
	bool GetHeight(IN const D3DXVECTOR3& Inpos, OUT D3DXVECTOR3& OutPos);
	int GetVertexCount();
	int GetIndexCount();
	int GetLineBufferIndexCount();

};