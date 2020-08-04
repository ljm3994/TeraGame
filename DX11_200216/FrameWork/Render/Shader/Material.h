#pragma once

#include "FrameWork/Buffer/Shader/MaterialBuffer.h"

class Material
{
	string Name;
	Shader* shader;

	unique_ptr<MaterialBuffer> Buffer;

	Texture* diffuseMap;
	string DiffuseName;
	Texture* specularMap;
	string SpecularName;
	Texture* normalMap;
	string NormalName;
public:
	Material();
	Material(string Key);
	Material(string Key, wstring Path);
	~Material();

	void Set();
	void Clear();
	void SetShader(Shader* value) { shader = value; }
	void SetShader(string Key);
	Shader* GetShader() { return shader; }

	MaterialBuffer* GetBuffer() { return Buffer.get(); }

	void SetDiffuseMap(string key);
	void SetDiffuseMap(string key, string Path);
	string GetDiffuseName() { return DiffuseName; }
	Texture* GetDiffuseMap() { return diffuseMap; }

	void SetSpecularMap(string key);
	void SetSpecularMap(string key, string Path);
	string GetSpecularName() { return SpecularName; }
	Texture* GetSpecularMap() { return specularMap; }

	void SetNormalMap(string key);
	void SetNormalMap(string key, string Path);
	string GetNormalName() { return NormalName; }
	Texture* GetNormalMap() { return normalMap; }

	string GetName() { return Name; }
	void SetName(string Value) { Name = Value; }
};