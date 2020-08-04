#include "Framework.h"
#include "MapExporter.h"
#include <FrameWork\Utility\Xml.h>
#include <FrameWork\Utility\BinaryHelper.h>

MapExporter::MapExporter(const aiScene* scene)
{
	this->scene = scene;

	Min = D3DXVECTOR3(1000000.0f, 1000000.0f, 1000000.0f);
	Max = D3DXVECTOR3(-1000000.0f, -1000000.0f, -1000000.0f);

	ReadMaterial();
	ReadMesh(scene->mRootNode);
}

MapExporter::~MapExporter()
{

}

void MapExporter::ZeroLocalCaluculation()
{

}

void MapExporter::ReadMesh(aiNode* node)
{
	if (!scene->HasMeshes())
		return;

	meshdescs.resize(scene->mNumMeshes);

	for (UINT i = 0; i < scene->mNumMeshes; i++)
	{
		aiMesh* srcMesh = scene->mMeshes[i];
		UINT matIndex = srcMesh->mMaterialIndex;
		
		meshdescs[i].MatName = scene->mMaterials[matIndex]->GetName().C_Str();

		for (UINT v = 0; v < srcMesh->mNumVertices; v++)
		{
			VertexTangent vertex;

			vertex.Position = srcMesh->HasPositions() ? D3DXVECTOR3(srcMesh->mVertices[v].x, srcMesh->mVertices[v].y, srcMesh->mVertices[v].z) : D3DXVECTOR3();
			vertex.UV = srcMesh->HasTextureCoords(0) ? D3DXVECTOR2(srcMesh->mTextureCoords[0][v].x, srcMesh->mTextureCoords[0][v].y) : D3DXVECTOR2();
			vertex.Normal = srcMesh->HasNormals() ? D3DXVECTOR3(srcMesh->mNormals[v].x, srcMesh->mNormals[v].y, srcMesh->mNormals[v].z) : D3DXVECTOR3();
			vertex.Tanjent = srcMesh->HasTangentsAndBitangents() ? D3DXVECTOR3(srcMesh->mTangents[v].x, srcMesh->mTangents[v].y, srcMesh->mTangents[v].z) : D3DXVECTOR3();
			
			meshdescs[i].vertices.emplace_back(vertex);

			if (Min.y > vertex.Position.y)
				Min.y = vertex.Position.y;
			if (Max.y < vertex.Position.y)
				Max.y = vertex.Position.y;

			if (Min.x > vertex.Position.x)
				Min.x = vertex.Position.x;
			if (Max.x < vertex.Position.x)
				Max.x = vertex.Position.x;

			if (Min.z > vertex.Position.z)
				Min.z = vertex.Position.z;
			if (Max.z < vertex.Position.z)
				Max.z = vertex.Position.z;
		}

		for (UINT f = 0; f < srcMesh->mNumFaces; f++)
		{
			aiFace& face = srcMesh->mFaces[f];

			for (UINT k = 0; k < face.mNumIndices; k++)
			{
				meshdescs[i].indices.emplace_back(face.mIndices[k]);
			}
		}
	}
}

void MapExporter::ReadMaterial()
{
	if (!scene->HasMaterials())
		return;
	
	materialinfos.resize(scene->mNumMaterials);
	for (UINT i = 0; i < scene->mNumMaterials; i++)
	{
		aiMaterial* material = scene->mMaterials[i];
		aiColor3D color;
		materialinfos[i].Name = material->GetName().C_Str();
		ZeroMemory(&color, sizeof(aiColor3D));
		material->Get(AI_MATKEY_COLOR_AMBIENT, color);
		float Opacity = 0.0f;
		material->Get(AI_MATKEY_OPACITY, Opacity);
		materialinfos[i].Ambient = D3DXCOLOR(color.r, color.g, color.b, Opacity);

		ZeroMemory(&color, sizeof(aiColor3D));
		material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
		materialinfos[i].Diffuse = D3DXCOLOR(color.r, color.g, color.b, 1.0f);

		ZeroMemory(&color, sizeof(aiColor3D));
		material->Get(AI_MATKEY_COLOR_SPECULAR, color);
		float Shininess = 0.0f;
		material->Get(AI_MATKEY_SHININESS, Shininess);
		materialinfos[i].Specular = D3DXCOLOR(color.r, color.g, color.b, Shininess);

		aiString file;
		aiReturn texFound;

		string str;
		file.Clear();
		texFound = material->GetTexture(aiTextureType_DIFFUSE, 0, &file);
		str = file.C_Str();
		if (texFound == AI_SUCCESS && str != "")
		{
			materialinfos[i].Diffusefile = str;
		}

		file.Clear();
		texFound = material->GetTexture(aiTextureType_SPECULAR, 0, &file);
		str = file.C_Str();
		if (texFound == AI_SUCCESS && str != "")
		{
			materialinfos[i].Specularfile = str;
		}

		file.Clear();
		texFound = material->GetTexture(aiTextureType_NORMALS, 0, &file);
		str = file.C_Str();
		if (texFound == AI_SUCCESS && str != "")
		{
			materialinfos[i].NormalFile = str;
		}
	}
}

void MapExporter::WriteMesh(string SaveFile, bool isOverWrite)
{
	if (!isOverWrite)
	{
		if (Helper::IsExistFile(SaveFile))
			return;
	}

	Helper::CreateFolders(SaveFile, true);

	BinaryWrite* w = new BinaryWrite(Helper::stringTowstring(&SaveFile));

	w->Float(&Min.x);
	w->Float(&Min.y);
	w->Float(&Min.z);

	w->Float(&Max.x);
	w->Float(&Max.y);
	w->Float(&Max.z);

	UINT Size = meshdescs.size();
	w->Uint(&Size);

	for (auto item : meshdescs)
	{
		w->String(&item.MatName);
		UINT verticesSize = item.vertices.size();
		w->Uint(&verticesSize);
		UINT ArraySize = sizeof(VertexTangent) * item.vertices.size();
		w->Byte(item.vertices.data(), &ArraySize);
		UINT indicesSize = item.indices.size();
		w->Uint(&indicesSize);
		ArraySize = sizeof(UINT) * item.indices.size();
		w->Byte(item.indices.data(), &ArraySize);
	}
	delete w;
}

void MapExporter::WriteMaterial(string SaveFile, bool isOverWrite)
{
	if (!isOverWrite)
	{
		if (Helper::IsExistFile(SaveFile))
			return;
	}

	Helper::CreateFolders(SaveFile, true);

	XmlDocument* document = new XmlDocument();
	XmlElement* root = document->NewElement("Materials");
	document->LinkEndChild(root);

	string folder = Helper::GetFileName(SaveFile);
	size_t index = folder.find_last_of('.');
	folder = folder.substr(0, index);

	for (auto item : materialinfos)
	{
		XmlElement* node = document->NewElement("Material");
		root->LinkEndChild(node);

		XmlElement* element = document->NewElement("Name");
		element->SetText(item.Name.c_str());
		node->LinkEndChild(element);

		element = document->NewElement("DiffuseFile");
		string temp = WriteTexture(scene, TextureDefaultPath + folder, item.Diffusefile);
		element->SetText(temp.c_str());
		node->LinkEndChild(element);

		element = document->NewElement("SpecularFile");
		temp = WriteTexture(scene, TextureDefaultPath + folder, item.Specularfile);
		element->SetText(temp.c_str());
		node->LinkEndChild(element);

		element = document->NewElement("NormalFile");
		temp = WriteTexture(scene, TextureDefaultPath + folder, item.NormalFile);
		element->SetText(temp.c_str());
		node->LinkEndChild(element);

		element = document->NewElement("Ambient");
		element->SetAttribute("R", item.Ambient.r);
		element->SetAttribute("G", item.Ambient.g);
		element->SetAttribute("B", item.Ambient.b);
		element->SetAttribute("A", item.Ambient.a);
		node->LinkEndChild(element);

		element = document->NewElement("Diffuse");
		element->SetAttribute("R", item.Diffuse.r);
		element->SetAttribute("G", item.Diffuse.g);
		element->SetAttribute("B", item.Diffuse.b);
		element->SetAttribute("A", item.Diffuse.a);
		node->LinkEndChild(element);

		element = document->NewElement("Specular");
		element->SetAttribute("R", item.Specular.r);
		element->SetAttribute("G", item.Specular.g);
		element->SetAttribute("B", item.Specular.b);
		element->SetAttribute("A", item.Specular.a);
		node->LinkEndChild(element);
	}

	document->SaveFile(SaveFile.c_str());

	delete document;
}

string MapExporter::WriteTexture(const aiScene* scene, string SavePath, string FilePath)
{
	if (SavePath.length() == 0)
		return "";

	string fileName = Helper::GetFileName(FilePath);
	const aiTexture* texture = scene->GetEmbeddedTexture(FilePath.c_str());

	if (texture)
	{
		if (texture->mHeight)
		{
			ID3D11Texture2D* dest;
			D3D11_TEXTURE2D_DESC destDesc = {};
			destDesc.Width = texture->mWidth;
			destDesc.Height = texture->mHeight;
			destDesc.MipLevels = 1;
			destDesc.ArraySize = 1;
			destDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			destDesc.SampleDesc.Count = 1;
			destDesc.SampleDesc.Quality = 0;
			destDesc.Usage = D3D11_USAGE_IMMUTABLE;

			D3D11_SUBRESOURCE_DATA subResource = {};
			subResource.pSysMem = texture->pcData;

			Engine->GetDevice()->CreateTexture2D(&destDesc, &subResource, &dest);

			D3DX11SaveTextureToFileA(Engine->GetContext(), dest, D3DX11_IFF_PNG, (SavePath + "/").c_str());
		}
		else
		{
			BinaryWrite w(Helper::stringTowstring(&(SavePath + "/" + texture->mFilename.C_Str())));
			w.Byte(texture->pcData, &texture->mWidth);
		}
	}

	return fileName;
}
