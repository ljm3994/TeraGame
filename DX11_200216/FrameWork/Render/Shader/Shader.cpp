#include "Framework.h"
#include "Shader.h"

void Shader::GetSOLayouts(D3D11_SO_DECLARATION_ENTRY** Desc, UINT* Count)
{
}

Shader::Shader(wstring _Pspath, ShaderCreateFlag shadercreateFlag, string VSName, string PSName, string CSName, string GSName)
{
	this->Pspath = L"Shader/" + _Pspath + L".hlsl";

	if ((shadercreateFlag & ShaderCreateFlag::SHADER_FLAG_VS) == ShaderCreateFlag::SHADER_FLAG_VS)
	{
		CreateVertexShader(VSName);
		CreateLayout();
	}
	if ((shadercreateFlag & ShaderCreateFlag::SHADER_FLAG_PS) == ShaderCreateFlag::SHADER_FLAG_PS)
	{
		CreatePixelShader(PSName);
	}
	if ((shadercreateFlag & ShaderCreateFlag::SHADER_FLAG_GS) == ShaderCreateFlag::SHADER_FLAG_GS)
	{
		CreateGeomtryShader(GSName);
	}
	if ((shadercreateFlag & ShaderCreateFlag::SHADER_FLAG_CS) == ShaderCreateFlag::SHADER_FLAG_CS)
	{
		CreateComputeShader(CSName);
	}
}

Shader::~Shader()
{
}

void Shader::CreateVertexShader(string VSName)
{
	if (!VertexBlob.Get())
	{
		UINT flags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG;

		HRESULT hr = D3DX11CompileFromFile(Pspath.c_str(), nullptr, nullptr,
			VSName.c_str(), "vs_5_0", flags, 0, nullptr, VertexBlob.GetAddressOf(), nullptr, nullptr);

		DEBUG(hr);

		hr = Engine->GetDevice()->CreateVertexShader(VertexBlob->GetBufferPointer(),
			VertexBlob->GetBufferSize(), nullptr, vertexShader.GetAddressOf());

		DEBUG(hr);
	}
}

void Shader::CreatePixelShader(string PSName)
{
	if (!pixelBlob.Get())
	{
		UINT flags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG;

		HRESULT hr = D3DX11CompileFromFile(Pspath.c_str(), nullptr, nullptr,
			PSName.c_str(), "ps_5_0", flags, 0, nullptr, pixelBlob.GetAddressOf(), nullptr, nullptr);

		if (SUCCEEDED(hr))
		{
			hr = Engine->GetDevice()->CreatePixelShader(pixelBlob->GetBufferPointer(),
				pixelBlob->GetBufferSize(), nullptr, pixelShader.GetAddressOf());

			DEBUG(hr);
		}
	}
}

void Shader::CreateComputeShader(string CSName)
{
	UINT flags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG;

	D3DX11CompileFromFile(Pspath.c_str(), nullptr, nullptr, CSName.c_str(), "cs_5_0", flags, 0, nullptr, ComputeBlob.GetAddressOf(), nullptr, nullptr);

	HRESULT hr = Engine->GetDevice()->CreateComputeShader(ComputeBlob->GetBufferPointer(), ComputeBlob->GetBufferSize(), nullptr, ComputeShader.GetAddressOf());

	DEBUG(hr);
}

void Shader::CreateGeomtryShader(string GSName)
{
	if (!GeometryBlob.Get())
	{
		UINT flags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG;

		HRESULT hr = D3DX11CompileFromFile(Pspath.c_str(), nullptr, nullptr,
			GSName.c_str(), "gs_5_0", flags, 0, nullptr, GeometryBlob.GetAddressOf(), nullptr, nullptr);

		DEBUG(hr);

		hr = Engine->GetDevice()->CreateGeometryShader(GeometryBlob->GetBufferPointer(),
			GeometryBlob->GetBufferSize(), nullptr, GeometryShader.GetAddressOf());

		DEBUG(hr);
	}
}

void Shader::CreateLayout()
{
	if (!inputLayout.Get())
	{
		ComPtr<ID3D11ShaderReflection> reflection;

		HRESULT hr = D3DReflect(VertexBlob->GetBufferPointer(), VertexBlob->GetBufferSize(),
			IID_ID3D11ShaderReflection, (void**)reflection.GetAddressOf());
		DEBUG(hr);

		D3D11_SHADER_DESC shaderDesc = {};
		reflection->GetDesc(&shaderDesc);

		vector<D3D11_INPUT_ELEMENT_DESC> inputLayouts = {};

		for (UINT i = 0; i < shaderDesc.InputParameters; i++)
		{
			D3D11_SIGNATURE_PARAMETER_DESC paramDesc = {};
			reflection->GetInputParameterDesc(i, &paramDesc);

			D3D11_INPUT_ELEMENT_DESC elementDesc;
			elementDesc.SemanticName = paramDesc.SemanticName;
			elementDesc.SemanticIndex = paramDesc.SemanticIndex;
			elementDesc.InputSlot = 0;
			elementDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
			elementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			elementDesc.InstanceDataStepRate = 0;

			if (paramDesc.Mask == 1)
			{
				if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
					elementDesc.Format = DXGI_FORMAT_R32_UINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
					elementDesc.Format = DXGI_FORMAT_R32_SINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
					elementDesc.Format = DXGI_FORMAT_R32_FLOAT;
			}
			else if (paramDesc.Mask <= 3)
			{
				if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
					elementDesc.Format = DXGI_FORMAT_R32G32_UINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
					elementDesc.Format = DXGI_FORMAT_R32G32_SINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
					elementDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
			}
			else if (paramDesc.Mask <= 7)
			{
				if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
					elementDesc.Format = DXGI_FORMAT_R32G32B32_UINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
					elementDesc.Format = DXGI_FORMAT_R32G32B32_SINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
					elementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
			}
			else if (paramDesc.Mask <= 15)
			{
				if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
					elementDesc.Format = DXGI_FORMAT_R32G32B32A32_UINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
					elementDesc.Format = DXGI_FORMAT_R32G32B32A32_SINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
					elementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			}

			string temp = paramDesc.SemanticName;
			if (temp == "POSITION")
				elementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;

			if (temp == "INSTANCE")
			{
				elementDesc.InputSlot = 1;
				elementDesc.InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
				elementDesc.InstanceDataStepRate = 1;
			}

			inputLayouts.push_back(elementDesc);
		}

		hr = Engine->GetDevice()->CreateInputLayout(inputLayouts.data(), inputLayouts.size(),
			VertexBlob->GetBufferPointer(), VertexBlob->GetBufferSize(), inputLayout.GetAddressOf());

		DEBUG(hr);

		reflection.Reset();
	}
}

void Shader::CreateGeomrtyStripOut(UINT* BufferStride, UINT StrideCount, string GSName)
{
	UINT flags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG;

	HRESULT hr = D3DX11CompileFromFile(Pspath.c_str(), nullptr, nullptr,
		GSName.c_str(), "gs_5_0", flags, 0, nullptr, &GeometryBlob, nullptr, nullptr);

	DEBUG(hr);
	D3D11_SO_DECLARATION_ENTRY* Temp = nullptr;
	UINT Count;
	GetSOLayouts(&Temp, &Count);
	hr = Engine->GetDevice()->CreateGeometryShaderWithStreamOutput(GeometryBlob->GetBufferPointer(),
		GeometryBlob->GetBufferSize(), Temp, Count, BufferStride, StrideCount, D3D11_SO_NO_RASTERIZED_STREAM, NULL, GeometryShader.GetAddressOf());

	DEBUG(hr);

	if (Temp)
	{
		delete[] Temp;
	}
}

void Shader::SetRender(VertexBuffer* Vertex, IndexBuffer* Index, D3D11_PRIMITIVE_TOPOLOGY Topology)
{
	ID3D11Buffer* nullBuffs[1] = { nullptr };
	uint32_t nullstrides[1] = { 0 };

	Engine->GetContext()->IASetInputLayout(inputLayout.Get());

	if (Vertex)
	{
		Vertex->VSSet();
	}
	else
		Engine->GetContext()->IASetVertexBuffers(0, 1, nullBuffs, nullstrides, nullstrides);

	if (Index)
	{
		Index->VSSet();
	}
	else
	{
		Engine->GetContext()->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);
	}

	Engine->GetContext()->IASetPrimitiveTopology(Topology);

	Engine->GetContext()->VSSetShader(vertexShader.Get(), nullptr, 0);
	Engine->GetContext()->GSSetShader(GeometryShader.Get(), nullptr, 0);
	Engine->GetContext()->PSSetShader(pixelShader.Get(), nullptr, 0);
}

void Shader::SetRender(D3D11_PRIMITIVE_TOPOLOGY Topology)
{
	Engine->GetContext()->IASetInputLayout(inputLayout.Get());

	Engine->GetContext()->IASetPrimitiveTopology(Topology);

	Engine->GetContext()->VSSetShader(vertexShader.Get(), nullptr, 0);
	Engine->GetContext()->GSSetShader(GeometryShader.Get(), nullptr, 0);
	Engine->GetContext()->PSSetShader(pixelShader.Get(), nullptr, 0);
}

void Shader::SetPS()
{
	Engine->GetContext()->PSSetShader(pixelShader.Get(), nullptr, 0);
}

void Shader::SetVS(bool isLayOut, D3D11_PRIMITIVE_TOPOLOGY Topology)
{
	if(isLayOut)
		Engine->GetContext()->IASetInputLayout(inputLayout.Get());
	else
		Engine->GetContext()->IASetInputLayout(nullptr);

	Engine->GetContext()->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
	Engine->GetContext()->IASetPrimitiveTopology(Topology);

	Engine->GetContext()->VSSetShader(vertexShader.Get(), nullptr, 0);
}

void Shader::SetCS()
{
	Engine->GetContext()->CSSetShader(ComputeShader.Get(), nullptr, 0);
}
