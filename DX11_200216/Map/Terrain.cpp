#include "Framework.h"
#include "Terrain.h"
#include "FrameWork\Utility\Xml.h"
#include <FrameWork\Utility\BinaryHelper.h>

Terrain::Terrain(string TerrainFilePath)
{
	TerrainModel = Engine->GetComponentModel<StaticModel>(TerrainFilePath);
	shader = Engine->AddShader("TerrainShader", L"TerrainShader");
}

Terrain::~Terrain()
{
	for (auto item : cells)
		delete item;
}

void Terrain::LoadTerrainCells()
{
	cells.resize(TerrainModel->GetMeshCount());

	for (UINT i = 0; i < TerrainModel->GetMeshCount(); i++)
	{
		cells[i] = new TerrainCell();
		auto mesh = TerrainModel->GetMeshData(i);
		cells[i]->ReadMapData(mesh, TerrainModel->GetMaterial(mesh->MatName));
	}
}

bool Terrain::GetHeight(IN const D3DXVECTOR3& Inpos, OUT D3DXVECTOR3& OutPos)
{
	bool returnval = false;
	for (UINT i = 0; i < TerrainModel->GetMeshCount(); i++)
	{
		float MinWidth, MaxWidth, MinDepth, MaxDepth;
		cells[i]->GetMinMaxDepth(MinDepth, MaxDepth);
		cells[i]->GetMinMaxWidth(MinWidth, MaxWidth);

		if (Inpos.x < MinWidth || Inpos.x > MaxWidth ||
			Inpos.z < MinDepth || Inpos.z > MaxDepth) continue;

		returnval = cells[i]->GetHeight(Inpos, OutPos);
	}
	return returnval;
}

void Terrain::Update()
{
	for (auto item : cells)
		item->Update();
}

void Terrain::Render()
{
	float MaxWidth, MinWidth, MaxHeight, MinHeight, MaxDepth, MinDepth;

	for (auto item : cells)
	{
		item->GetMinMaxWidth(MinWidth, MaxWidth);
		item->GetMinMaxHeight(MinHeight, MaxHeight);
		item->GetMinMaxDepth(MinDepth, MaxDepth);

		if (Engine->GetFrustum()->RectangleCheck(MaxWidth, MinWidth, MaxHeight, MinHeight, MaxDepth, MinDepth))
		{
			item->Render(shader);
			item->RenderLine();
		}
	}
}
