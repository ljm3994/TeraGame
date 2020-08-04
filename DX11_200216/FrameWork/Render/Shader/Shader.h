#pragma once
#include "FrameWork/Buffer/Shader/VertexBuffer.h"
#include "FrameWork/Buffer/Shader/IndexBuffer.h"
#include <type_traits>

ENUM_FLAGS(ShaderCreateFlag)
{
	SHADER_FLAG_VS = 0x01 << 0,
	SHADER_FLAG_PS = 0x01 << 1,
	SHADER_FLAG_GS = 0x01 << 2,
	SHADER_FLAG_CS = 0x01 << 3
};

class Shader
{
	friend class DXEngine;
	wstring Pspath;

	ComPtr<ID3D11VertexShader> vertexShader;
	ComPtr<ID3D11PixelShader> pixelShader;
	ComPtr<ID3D11GeometryShader> GeometryShader;
	ComPtr<ID3D11ComputeShader> ComputeShader;

	ComPtr<ID3D11InputLayout> inputLayout;

	ComPtr<ID3DBlob> VertexBlob;
	ComPtr<ID3DBlob> GeometryBlob;
	ComPtr<ID3DBlob> pixelBlob;
	ComPtr<ID3DBlob> ComputeBlob;

	void GetSOLayouts(D3D11_SO_DECLARATION_ENTRY** Desc, UINT* Count);
	Shader(wstring _Pspath, ShaderCreateFlag shadercreateFlag = ShaderCreateFlag::SHADER_FLAG_VS | ShaderCreateFlag::SHADER_FLAG_PS, string VSName = "VS", string PSName = "PS", string CSName = "CS", string GSName = "GS");
	void CreateVertexShader(string VSName = "VS");
	void CreatePixelShader(string PSName = "PS");
	void CreateComputeShader(string CSName = "CS");
	void CreateGeomtryShader(string GSName = "GS");
	void CreateLayout();
public:
	~Shader();
	void CreateGeomrtyStripOut(UINT* BufferStride, UINT StrideCount, string GSName = "GS");
	void SetRender(VertexBuffer* Vertex, IndexBuffer* Index = nullptr, D3D11_PRIMITIVE_TOPOLOGY Topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	void SetRender(D3D11_PRIMITIVE_TOPOLOGY Topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	void SetPS();
	void SetVS(bool isLayOut = false, D3D11_PRIMITIVE_TOPOLOGY Topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	void SetCS();
};