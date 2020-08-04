#include "Framework.h"
#include "SkinnedMesh.h"
#include "AssimpHelper.h"
#include <FrameWork\Utility\BinaryHelper.h>
#include <FrameWork\Utility\Xml.h>
#include "FrameWork\Assimp\Animator.h"

SkinnedMesh::SkinnedMesh(const aiScene* scene, wstring FileName)
{
	Min = D3DXVECTOR3(1000000.0f , 1000000.0f , 1000000.0f);
	Max = D3DXVECTOR3(-1000000.0f , -1000000.0f , -1000000.0f);

	this->scene = scene;
	ReadMatarial();
	ReadNodeData(scene->mRootNode, -1, -1);
	ReadMeshData(scene->mRootNode);
}

SkinnedMesh::~SkinnedMesh()
{
	for (auto item : nodes)
		delete item;

	for (auto item : bones)
		delete item;
}

void SkinnedMesh::ReadAnimation(const aiScene* scene, bool isRootLock)
{
	if (!animation)
		animation = new Animator();

	animation->ExtractAnimation(scene);
	animation->CreateBoneAnimation(boneMap, bones, nodes, isRootLock);
}

D3DXMATRIX SkinnedMesh::GetFindBoneWorldMatrix(string Name)
{
	for (auto item : nodes)
	{
		if (item->Name == Name)
			return item->Transform;
	}

	D3DXMATRIX mat;
	D3DXMatrixIdentity(&mat);
	return mat;
}

void SkinnedMesh::WriteMaterialFile(string SaveFile, bool isOverWrite)
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

	auto it = Materials.begin();

	for (; it != Materials.end(); it++)
	{
		XmlElement* node = document->NewElement("Material");
		root->LinkEndChild(node);

		XmlElement* element = document->NewElement("Name");
		element->SetText(it->second->Name.c_str());
		node->LinkEndChild(element);

		element = document->NewElement("DiffuseFile");
		string temp = WriteTexture(scene, TextureDefaultPath + folder, it->second->Diffusefile);
		element->SetText(temp.c_str());
		node->LinkEndChild(element);

		element = document->NewElement("SpecularFile");
		temp = WriteTexture(scene, TextureDefaultPath + folder, it->second->Specularfile);
		element->SetText(temp.c_str());
		node->LinkEndChild(element);

		element = document->NewElement("NormalFile");
		temp = WriteTexture(scene, TextureDefaultPath + folder, it->second->NormalFile);
		element->SetText(temp.c_str());
		node->LinkEndChild(element);

		element = document->NewElement("Ambient");
		element->SetAttribute("R", it->second->Ambient.r);
		element->SetAttribute("G", it->second->Ambient.g);
		element->SetAttribute("B", it->second->Ambient.b);
		element->SetAttribute("A", it->second->Ambient.a);
		node->LinkEndChild(element);

		element = document->NewElement("Diffuse");
		element->SetAttribute("R", it->second->Diffuse.r);
		element->SetAttribute("G", it->second->Diffuse.g);
		element->SetAttribute("B", it->second->Diffuse.b);
		element->SetAttribute("A", it->second->Diffuse.a);
		node->LinkEndChild(element);

		element = document->NewElement("Specular");
		element->SetAttribute("R", it->second->Specular.r);
		element->SetAttribute("G", it->second->Specular.g);
		element->SetAttribute("B", it->second->Specular.b);
		element->SetAttribute("A", it->second->Specular.a);
		node->LinkEndChild(element);

		delete it->second;
	}

	document->SaveFile(SaveFile.c_str());

	delete document;
}

void SkinnedMesh::WriteMeshFile(string SaveFile, bool isOverWrite)
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

	UINT Size = bones.size();
	w->Uint(&Size);
	for (AssimpBone* bone : bones)
	{
		w->String(&bone->Name);
		w->Matrix(&bone->Offset);
		w->Matrix(&bone->transform);
	}

	Size = nodes.size();
	w->Uint(&Size);
	for (AssimpNode* node : nodes)
	{
		w->String(&node->Name);
		w->Int(&node->Index);
		w->Int(&node->Parent);
		w->Matrix(&node->Transform);
	}

	Size = meshes.size();
	w->Uint(&Size);
	for (Mesh* mesh : meshes)
	{
		w->String(&mesh->MaterialName);

		UINT verticesSize = mesh->vertices.size();
		w->Uint(&verticesSize);
		UINT ArraySize = sizeof(VertexModelData) * mesh->vertices.size();
		w->Byte(mesh->vertices.data(), &ArraySize);
		UINT indicesSize = mesh->indics.size();
		w->Uint(&indicesSize);
		ArraySize = sizeof(UINT) * mesh->indics.size();
		w->Byte(mesh->indics.data(), &ArraySize);

		delete mesh;
	}

	delete w;
}

void SkinnedMesh::WriteAnimationFile(string SaveFile, bool isOverWrite)
{
	if (animation)
		animation->SaveAnimationFile(SaveFile, isOverWrite);
}

void SkinnedMesh::ReadMatarial()
{
	for (UINT i = 0; i < scene->mNumMaterials; i++)
	{
		aiMaterial* material = scene->mMaterials[i];
		MaterialInfo* mat = new MaterialInfo();

		aiColor3D color;
		mat->Name = material->GetName().C_Str();
		ZeroMemory(&color, sizeof(aiColor3D));
		material->Get(AI_MATKEY_COLOR_AMBIENT, color);
		float Opacity = 0.0f;
		material->Get(AI_MATKEY_OPACITY, Opacity);
		mat->Ambient = D3DXCOLOR(color.r, color.g, color.b, Opacity);

		ZeroMemory(&color, sizeof(aiColor3D));
		material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
		mat->Diffuse = D3DXCOLOR(color.r, color.g, color.b, 1.0f);

		ZeroMemory(&color, sizeof(aiColor3D));
		material->Get(AI_MATKEY_COLOR_SPECULAR, color);
		float Shininess = 0.0f;
		material->Get(AI_MATKEY_SHININESS, Shininess);
		mat->Specular = D3DXCOLOR(color.r, color.g, color.b, Shininess);

		aiString file;
		aiReturn texFound;

		string str;
		file.Clear();
		texFound = material->GetTexture(aiTextureType_DIFFUSE, 0, &file);
		str = file.C_Str();
		if (texFound == AI_SUCCESS && str != "")
		{
			mat->Diffusefile = str;
		}

		file.Clear();
		texFound = material->GetTexture(aiTextureType_SPECULAR, 0, &file);
		str = file.C_Str();
		if (texFound == AI_SUCCESS && str != "")
		{
			mat->Specularfile = str;
		}

		file.Clear();
		texFound = material->GetTexture(aiTextureType_NORMALS, 0, &file);
		str = file.C_Str();
		if (texFound == AI_SUCCESS && str != "")
		{
			mat->NormalFile = str;
		}

		Materials.insert(make_pair(material->GetName().C_Str(), mat));
	}
}

void SkinnedMesh::ReadNodeData(aiNode* node, int index, int parent)
{
	AssimpNode* asnode = new AssimpNode();
	asnode->Index = index;
	asnode->Parent = parent;
	asnode->Name = node->mName.C_Str();

	D3DXMATRIX transform = D3DXMATRIX(node->mTransformation[0]);
	D3DXMatrixTranspose(&asnode->Transform, &transform);

	nodes.emplace_back(asnode);

	for (UINT i = 0; i < node->mNumChildren; i++)
		ReadNodeData(node->mChildren[i], nodes.size(), index);
}

void SkinnedMesh::ReadMeshData(aiNode* node)
{
	for (UINT i = 0; i < node->mNumMeshes; i++)
	{
		Mesh* mesh = new Mesh();
		mesh->Name = node->mName.C_Str();

		UINT index = node->mMeshes[i];
		aiMesh* srcMesh = scene->mMeshes[index];

		aiMaterial* mat = scene->mMaterials[srcMesh->mMaterialIndex];
		mesh->MaterialName = mat->GetName().C_Str();

		UINT startVertex = mesh->vertices.size();

		vector<BoneWeight> boneweight;
		boneweight.resize(srcMesh->mNumVertices);

		ReadBoneData(srcMesh, boneweight);

		for (UINT v = 0; v < srcMesh->mNumVertices; v++)
		{
			VertexModelData vertex;

			vertex.Position = srcMesh->HasPositions() ? D3DXVECTOR3(srcMesh->mVertices[v].x, srcMesh->mVertices[v].y, srcMesh->mVertices[v].z) : D3DXVECTOR3();
			vertex.Uv = srcMesh->HasTextureCoords(0) ? D3DXVECTOR2(srcMesh->mTextureCoords[0][v].x, srcMesh->mTextureCoords[0][v].y) : D3DXVECTOR2();
			vertex.Normal = srcMesh->HasNormals() ? D3DXVECTOR3(srcMesh->mNormals[v].x, srcMesh->mNormals[v].y, srcMesh->mNormals[v].z) : D3DXVECTOR3();
			vertex.Tangent = srcMesh->HasTangentsAndBitangents() ? D3DXVECTOR3(srcMesh->mTangents[v].x, srcMesh->mTangents[v].y, srcMesh->mTangents[v].z) : D3DXVECTOR3();

			if (vertex.Position.x > Max.x)
				Max.x = vertex.Position.x;
			if (vertex.Position.x < Min.x)
				Min.x = vertex.Position.x;

			if (vertex.Position.y > Max.y)
				Max.y = vertex.Position.y;
			if (vertex.Position.y < Min.y)
				Min.y = vertex.Position.y;

			if (vertex.Position.z > Max.z)
				Max.z = vertex.Position.z;
			if (vertex.Position.z < Min.z)
				Min.z = vertex.Position.z;

			//D3DXVec3Minimize(&Min, &Min, &D3DXVECTOR3(vertex.Position.x, vertex.Position.y, vertex.Position.z));
			//D3DXVec3Maximize(&Max, &Max, &D3DXVECTOR3(vertex.Position.x, vertex.Position.y, vertex.Position.z));

			if (!boneweight.empty())
			{
				boneweight[v].Normalize();

				vertex.Indices.x = static_cast<float>(boneweight[v].BoneIndex[0]);
				vertex.Indices.y = static_cast<float>(boneweight[v].BoneIndex[1]);
				vertex.Indices.z = static_cast<float>(boneweight[v].BoneIndex[2]);
				vertex.Indices.w = static_cast<float>(boneweight[v].BoneIndex[3]);

				vertex.Weights.x = boneweight[v].Weights[0];
				vertex.Weights.y = boneweight[v].Weights[1];
				vertex.Weights.z = boneweight[v].Weights[2];
				vertex.Weights.w = boneweight[v].Weights[3];
			}

			mesh->vertices.emplace_back(vertex);
		}

		for (UINT f = 0; f < srcMesh->mNumFaces; f++)
		{
			aiFace& face = srcMesh->mFaces[f];

			for (UINT k = 0; k < face.mNumIndices; k++)
			{
				mesh->indics.emplace_back(face.mIndices[k]);
				mesh->indics.back() += startVertex;
			}
		}

		meshes.emplace_back(mesh);

		boneweight.clear();
		boneweight.shrink_to_fit();
	}

	for (UINT i = 0; i < node->mNumChildren; i++)
		ReadMeshData(node->mChildren[i]);
}

void SkinnedMesh::ReadBoneData(aiMesh* mesh, vector<BoneWeight>& vertex_weights)
{
	for (UINT i = 0; i < mesh->mNumBones; i++)
	{
		UINT boneIndex = 0;
		string Name = mesh->mBones[i]->mName.C_Str();

		if (boneMap.count(Name) == 0)
		{
			boneIndex = boneMap.size();

			boneMap[Name] = boneIndex;
			AssimpBone* bone = new AssimpBone();
			bone->Name = Name;
			bone->transform = GetFindBoneWorldMatrix(Name);
			bone->Offset = AssimpHelper::ToMatrix(mesh->mBones[i]->mOffsetMatrix);

			bones.emplace_back(bone);
		}
		else
		{
			boneIndex = boneMap[Name];
		}

		for (UINT j = 0; j < mesh->mBones[i]->mNumWeights; j++)
		{
			UINT vertexID = mesh->mBones[i]->mWeights[j].mVertexId;
			vertex_weights[vertexID].Add(boneIndex, mesh->mBones[i]->mWeights[j].mWeight);
		}
	}
}

string SkinnedMesh::WriteTexture(const aiScene* scene, string SavePath, string FilePath)
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

