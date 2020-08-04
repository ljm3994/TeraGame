#include "Framework.h"
#include "ModelInstance.h"
#include "FrameWork\Utility\Xml.h"
#include "FrameWork\Render\Shader\Material.h"

ModelInstance::ModelInstance(string FileName, string ShaderName, bool isCollision, UINT instanceCount)
	: isCollision(isCollision), instanceCount(instanceCount)
{
	shader = Engine->AddShader(ShaderName, Helper::stringTowstring(&ShaderName));
	buffer = new InstanceBuffer();
	ReadMaterial(DefaultMaterialPath + FileName + ".mat");

	Worlds.resize(instanceCount);
	worldmat = new D3DXMATRIX[instanceCount];
	for (UINT i = 0; i < instanceCount; i++)
	{
		Worlds[i] = new Transform();
	}

	for (UINT i = 0; i < instanceCount; i++)
	{
		D3DXMatrixIdentity(&worldmat[i]);
	}
	Instancebuffer = new VertexBuffer(worldmat, sizeof(D3DXMATRIX), instanceCount);
}

ModelInstance::~ModelInstance()
{
	for (auto item : materials)
		delete item.second;

	for (auto item : Worlds)
		delete item;

	delete Instancebuffer;
	delete buffer;
	delete[] worldmat;
}

void ModelInstance::UpdateTransform()
{
	for (UINT i = 0; i < instanceCount; i++)
	{
		if (buffer->GetIsActive(i))
		{
			Worlds[i]->UpdateWorld();
			D3DXMATRIX mat = *Worlds[i]->GetWorld();
			D3DXMatrixTranspose(&mat, &mat);
			memcpy(&worldmat[i], &mat, sizeof(D3DXMATRIX));
		}
		else
		{
			D3DXMATRIX mat = D3DXMATRIX(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
			memcpy(&worldmat[i], &mat, sizeof(D3DXMATRIX));
		}
	}

	Instancebuffer->Update(worldmat, instanceCount);
}

bool ModelInstance::GetActive(UINT InstanceID)
{
	return buffer->GetIsActive(InstanceID);
}

void ModelInstance::SetActive(UINT InstanceID, bool val)
{
	buffer->SetIsActive(InstanceID, val);
}


void ModelInstance::ReadMaterial(string FileName)
{
	XmlDocument* document = new XmlDocument();
	document->LoadFile(FileName.c_str());

	XmlElement* root = document->FirstChildElement();
	XmlElement* matNode = root->FirstChildElement();

	string folder = Helper::GetDirectoryName(FileName);
	size_t index = folder.find_last_of('/');
	folder = folder.substr(0, index);
	index = folder.find_last_of('/');
	folder = folder.substr(index + 1, folder.length());
	folder += "/";

	do
	{
		XmlElement* node = matNode->FirstChildElement();

		Material* material = new Material();
		material->SetName(node->GetText());

		node = node->NextSiblingElement();

		if (node->GetText())
		{
			string file = node->GetText();
			if (file != "" && Helper::IsExistFile(TextureDefaultPath + folder + file))
			{
				material->SetDiffuseMap(file, TextureDefaultPath + folder + file);
			}
		}

		node = node->NextSiblingElement();

		if (node->GetText())
		{
			string file = node->GetText();
			if (file != "" && Helper::IsExistFile(TextureDefaultPath + folder + file))
			{
				material->SetSpecularMap(TextureDefaultPath + folder + file);
			}
		}

		node = node->NextSiblingElement();

		if (node->GetText())
		{
			string file = node->GetText();
			if (file != "" && Helper::IsExistFile(TextureDefaultPath + folder + file))
			{
				material->SetNormalMap(TextureDefaultPath + folder + file);
			}
		}

		D3DXCOLOR color;

		node = node->NextSiblingElement();
		color.r = node->FloatAttribute("R");
		color.g = node->FloatAttribute("G");
		color.b = node->FloatAttribute("B");
		color.a = node->FloatAttribute("A");
		material->GetBuffer()->data.ambient = color;

		node = node->NextSiblingElement();
		color.r = node->FloatAttribute("R");
		color.g = node->FloatAttribute("G");
		color.b = node->FloatAttribute("B");
		color.a = node->FloatAttribute("A");
		material->GetBuffer()->data.diffuse = color;

		node = node->NextSiblingElement();
		color.r = node->FloatAttribute("R");
		color.g = node->FloatAttribute("G");
		color.b = node->FloatAttribute("B");
		color.a = node->FloatAttribute("A");
		material->GetBuffer()->data.specular = color;

		materials.insert(make_pair(material->GetName(), material));

		matNode = matNode->NextSiblingElement();
	} while (matNode != nullptr);
}

void ModelInstance::CreateReductionTarget(UINT Width, UINT Height)
{
}

Transform* ModelInstance::GetTransform(UINT instanceID)
{
	if (Worlds.size() > instanceID)
	{
		return Worlds[instanceID];
	}
	return nullptr;
}

InstanceBuffer* ModelInstance::GetBuffer()
{
	return buffer;
}
