#pragma once
#include "TerrainCell.h"
#include <FrameWork\Render\Model\StaticModel.h>

class Terrain
{
	Shader* shader;
	vector<TerrainCell*> cells;
	StaticModel* TerrainModel;
public:
	Terrain(string TerrainFilePath);
	~Terrain();

	void LoadTerrainCells();
	bool GetHeight(IN const D3DXVECTOR3& Inpos, OUT D3DXVECTOR3& OutPos);
	void Update();
	void Render();
};