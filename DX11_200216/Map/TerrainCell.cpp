#include "Framework.h"
#include "TerrainCell.h"
#include <FrameWork\Utility\BinaryHelper.h>
#include <FrameWork\Utility\Xml.h>

TerrainCell::TerrainCell()
{
}

TerrainCell::~TerrainCell()
{
	delete[] input;
	delete[] output;
	delete computebuffer;
	delete raybuffer;
}

void TerrainCell::ReadMapData(StaticMeshDesc* desc, Material* mat)
{
	terrainmesh = desc;
	material = mat;

	lineShader = Engine->AddShader("Test", L"Test");
	PickShader = Engine->AddShader("Intersection", L"Intersection", ShaderCreateFlag::SHADER_FLAG_CS);

	vertexList.resize(terrainmesh->vertices.size());
	UINT index = 0;
	input = new InuputDesc[terrainmesh->indices.size()];
	for (UINT i = 0; i < terrainmesh->vertices.size(); i++, index++)
	{
		terrainmesh->vertices[i].Position *= 0.01f;
		vertexList[i].x = terrainmesh->vertices[i].Position.x;
		vertexList[i].y = terrainmesh->vertices[i].Position.y;
		vertexList[i].z = terrainmesh->vertices[i].Position.z;
	}

	for (UINT i = 0; i < terrainmesh->indices.size() / 3; i++)
	{
		UINT index0 = terrainmesh->indices[i * 3 + 0];
		UINT index1 = terrainmesh->indices[i * 3 + 1];
		UINT index2 = terrainmesh->indices[i * 3 + 2];

		input[i].v0 = terrainmesh->vertices[index0].Position;
		input[i].v1 = terrainmesh->vertices[index1].Position;
		input[i].v2 = terrainmesh->vertices[index2].Position;

		input[i].index = i;
	}

	size = terrainmesh->indices.size() / 3;
	computebuffer = new ComputeDataBuffer(input, sizeof(InuputDesc), size, sizeof(OutputDesc), size);
	vertexbuffers = make_unique<VertexBuffer>(VertexBuffer(terrainmesh->vertices.data(), sizeof(VertexTangent), terrainmesh->vertices.size()));
	indexbuffers = make_unique<IndexBuffer>(IndexBuffer(terrainmesh->indices.data(), terrainmesh->indices.size()));
	raybuffer = new RayBuffer();
	output = new OutputDesc[size];

	CalculateCellDimensions();
	CreateLine(D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f), D3DXVECTOR2(MinWidth, MaxWidth), D3DXVECTOR2(MinHeight, MaxHeight), D3DXVECTOR2(MinDepth, MaxDepth));
}

void TerrainCell::CalculateCellDimensions()
{
	MaxWidth = -1000000.0f;
	MaxHeight = -1000000.0f;
	MaxDepth = -1000000.0f;

	MinWidth = 1000000.0f;
	MinHeight = 1000000.0f;
	MinDepth = 1000000.0f;

	for (UINT i = 0; i < terrainmesh->vertices.size(); i++)
	{
		float width = vertexList[i].x;
		float height = vertexList[i].y;
		float depth = vertexList[i].z;

		if (width > MaxWidth)
			MaxWidth = width;
		if (width < MinWidth)
			MinWidth = width;

		if (height > MaxHeight)
			MaxHeight = height;
		if (height < MinHeight)
			MinHeight = height;

		if (depth > MaxDepth)
			MaxDepth = depth;
		if (depth < MinDepth)
			MinDepth = depth;
	}

	//가운데 좌표 계산
	CenterPosition.x = (MaxWidth - MinWidth) + MinWidth;
	CenterPosition.y = (MaxHeight - MinHeight) + MinHeight;
	CenterPosition.z = (MaxDepth - MinDepth) + MinDepth;
}

void TerrainCell::CreateLine(D3DXCOLOR LineColor, D3DXVECTOR2 Width, D3DXVECTOR2 Height, D3DXVECTOR2 Depth)
{
	D3DXCOLOR linecolor = LineColor;

	//정점 갯수는?
	int vertexCount = 24;

	int indexCount = vertexCount;

	linevertices.resize(vertexCount);
	lineindices.resize(indexCount);

	int index = 0;
	//수평선
	linevertices[index].Position = D3DXVECTOR3(Width.x, Height.x, Depth.x);
	linevertices[index].Color = linecolor;
	lineindices[index] = index;
	index++;

	linevertices[index].Position = D3DXVECTOR3(Width.y, Height.x, Depth.x);
	linevertices[index].Color = linecolor;
	lineindices[index] = index;
	index++;

	linevertices[index].Position = D3DXVECTOR3(Width.x, Height.x, Depth.y);
	linevertices[index].Color = linecolor;
	lineindices[index] = index;
	index++;

	linevertices[index].Position = D3DXVECTOR3(Width.y, Height.x, Depth.y);
	linevertices[index].Color = linecolor;
	lineindices[index] = index;
	index++;

	linevertices[index].Position = D3DXVECTOR3(Width.x, Height.x, Depth.x);
	linevertices[index].Color = linecolor;
	lineindices[index] = index;
	index++;

	linevertices[index].Position = D3DXVECTOR3(Width.x, Height.x, Depth.y);
	linevertices[index].Color = linecolor;
	lineindices[index] = index;
	index++;

	linevertices[index].Position = D3DXVECTOR3(Width.y, Height.x, Depth.x);
	linevertices[index].Color = linecolor;
	lineindices[index] = index;
	index++;

	linevertices[index].Position = D3DXVECTOR3(Width.y, Height.x, Depth.y);
	linevertices[index].Color = linecolor;
	lineindices[index] = index;
	index++;

	linevertices[index].Position = D3DXVECTOR3(Width.x, Height.y, Depth.x);
	linevertices[index].Color = linecolor;
	lineindices[index] = index;
	index++;

	linevertices[index].Position = D3DXVECTOR3(Width.y, Height.y, Depth.x);
	linevertices[index].Color = linecolor;
	lineindices[index] = index;
	index++;

	linevertices[index].Position = D3DXVECTOR3(Width.x, Height.y, Depth.y);
	linevertices[index].Color = linecolor;
	lineindices[index] = index;
	index++;

	linevertices[index].Position = D3DXVECTOR3(Width.y, Height.y, Depth.y);
	linevertices[index].Color = linecolor;
	lineindices[index] = index;
	index++;

	linevertices[index].Position = D3DXVECTOR3(Width.x, Height.y, Depth.x);
	linevertices[index].Color = linecolor;
	lineindices[index] = index;
	index++;

	linevertices[index].Position = D3DXVECTOR3(Width.x, Height.y, Depth.y);
	linevertices[index].Color = linecolor;
	lineindices[index] = index;
	index++;

	linevertices[index].Position = D3DXVECTOR3(Width.y, Height.y, Depth.x);
	linevertices[index].Color = linecolor;
	lineindices[index] = index;
	index++;

	linevertices[index].Position = D3DXVECTOR3(Width.y, Height.y, Depth.y);
	linevertices[index].Color = linecolor;
	lineindices[index] = index;
	index++;

	//수직선
	linevertices[index].Position = D3DXVECTOR3(Width.y, Height.y, Depth.y);
	linevertices[index].Color = linecolor;
	lineindices[index] = index;
	index++;

	linevertices[index].Position = D3DXVECTOR3(Width.y, Height.x, Depth.y);
	linevertices[index].Color = linecolor;
	lineindices[index] = index;
	index++;

	linevertices[index].Position = D3DXVECTOR3(Width.x, Height.y, Depth.y);
	linevertices[index].Color = linecolor;
	lineindices[index] = index;
	index++;

	linevertices[index].Position = D3DXVECTOR3(Width.x, Height.x, Depth.y);
	linevertices[index].Color = linecolor;
	lineindices[index] = index;
	index++;

	linevertices[index].Position = D3DXVECTOR3(Width.y, Height.y, Depth.x);
	linevertices[index].Color = linecolor;
	lineindices[index] = index;
	index++;

	linevertices[index].Position = D3DXVECTOR3(Width.y, Height.x, Depth.x);
	linevertices[index].Color = linecolor;
	lineindices[index] = index;
	index++;

	linevertices[index].Position = D3DXVECTOR3(Width.x, Height.y, Depth.x);
	linevertices[index].Color = linecolor;
	lineindices[index] = index;
	index++;

	linevertices[index].Position = D3DXVECTOR3(Width.x, Height.x, Depth.x);
	linevertices[index].Color = linecolor;
	lineindices[index] = index;

	linevertexbuffers = make_unique<VertexBuffer>(VertexBuffer(linevertices.data(), sizeof(VertexColor), linevertices.size()));
	lineindexbuffers = make_unique<IndexBuffer>(IndexBuffer(lineindices.data(), lineindices.size()));
}

void TerrainCell::Update()
{
	UpdateWorld();
}

void TerrainCell::Render(Shader * shader)
{
	material->Set();

	shader->SetRender(vertexbuffers.get(), indexbuffers.get());
	SetVS();

	Engine->GetContext()->DrawIndexed(terrainmesh->indices.size(), 0, 0);
}

void TerrainCell::GetMinMaxWidth(float& MinWidth, float& MaxWidth)
{
	MinWidth = this->MinWidth;
	MaxWidth = this->MaxWidth;
}

void TerrainCell::GetMinMaxHeight(float& MinHeight, float& MaxHeight)
{
	MinHeight = this->MinHeight;
	MaxHeight = this->MaxHeight;
}

void TerrainCell::GetMinMaxDepth(float& MinDepth, float& MaxDepth)
{
	MinDepth = this->MinDepth;
	MaxDepth = this->MaxDepth;
}

void TerrainCell::RenderLine()
{
	lineShader->SetRender(linevertexbuffers.get(), lineindexbuffers.get(), D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	SetVS();

	Engine->GetContext()->DrawIndexed(lineindices.size(), 0, 0);
}

void TerrainCell::CreateTile()
{

}
// 지형의 높이를 가져온다.
bool TerrainCell::GetHeight(IN const D3DXVECTOR3& Inpos, OUT D3DXVECTOR3& OutPos)
{
	Ray ray;
	ray.position = Inpos;  // 현재 위치
	D3DXVec3Normalize(&ray.direction, &(D3DXVECTOR3(Inpos.x, 0.0f, Inpos.z) - Inpos)); // 방향
	raybuffer->SetRay(ray);  //레이버퍼 세팅
	raybuffer->SetSize(size);
	PickShader->SetCS();

	raybuffer->SetCSBuffer(0);

	Engine->GetContext()->CSSetShaderResources(0, 1, computebuffer->GetSrv());
	Engine->GetContext()->CSSetUnorderedAccessViews(0, 1, computebuffer->GetUav(), nullptr);

	UINT x = ceil((float)size / 1024.0f);

	Engine->GetContext()->Dispatch(x, 1, 1);  //컴퓨트 세이더를 실행한다.

	computebuffer->Copy(output, sizeof(OutputDesc) * size);  // 컴퓨트로 가져온 값 복사

	ID3D11ShaderResourceView* Empty = nullptr;
	Engine->GetContext()->CSSetShaderResources(0, 1, &Empty);
	ID3D11UnorderedAccessView* uavEmpty = nullptr;
	Engine->GetContext()->CSSetUnorderedAccessViews(0, 1, &uavEmpty, nullptr);
	ID3D11Buffer* emptyBuffer = nullptr;
	Engine->GetContext()->CSSetConstantBuffers(0, 1, &emptyBuffer);
	ID3D11ComputeShader* EmptyCsshader = nullptr;
	Engine->GetContext()->CSSetShader(EmptyCsshader, nullptr, 0);

	for (UINT i = 0; i < size; i++)
	{
		OutputDesc temp = output[i];

		if (temp.picked == 1)  //충돌 지점을 찾았다
		{
			D3DXVECTOR3 temppos = ray.position + ray.direction * temp.distance; // 높이 값을 계산한다.
			OutPos = temppos;
			return true;
		}
	}

	return false;
}

int TerrainCell::GetVertexCount()
{
	return 0;
}

int TerrainCell::GetIndexCount()
{
	return 0;
}

int TerrainCell::GetLineBufferIndexCount()
{
	return 0;
}
