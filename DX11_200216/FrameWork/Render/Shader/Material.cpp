#include "Framework.h"
#include "Material.h"

Material::Material()
	: shader(nullptr), diffuseMap(nullptr), specularMap(nullptr), normalMap(nullptr)
{
	Buffer = make_unique<MaterialBuffer>(MaterialBuffer());
}

Material::Material(string Key)
	: diffuseMap(nullptr), specularMap(nullptr), normalMap(nullptr)
{
	shader = Engine->FindShader(Key);
	Buffer = make_unique<MaterialBuffer>(MaterialBuffer());
}

Material::Material(string Key, wstring Path)
	: diffuseMap(nullptr), specularMap(nullptr), normalMap(nullptr)
{
	shader = Engine->AddShader(Key, Path);
	Buffer = make_unique<MaterialBuffer>(MaterialBuffer());
}

Material::~Material()
{
}

void Material::Set()
{
	if (diffuseMap != nullptr)
	{
		Buffer->SetisDiffuseMap(true);
		diffuseMap->PSSet(0);
	}
	else
		Buffer->SetisDiffuseMap(false);

	if (specularMap != nullptr)
	{
		Buffer->SetisSpecMap(true);
		specularMap->PSSet(1);
	}
	else
		Buffer->SetisSpecMap(false);

	if (normalMap != nullptr)
	{
		Buffer->SetisNormalMap(true);
		normalMap->PSSet(2);
	}
	else
		Buffer->SetisNormalMap(false);

	Buffer->SetPSBuffer(1);
}

void Material::Clear()
{
	if (diffuseMap != nullptr)
		diffuseMap->Clear(0);

	if (specularMap != nullptr)
		specularMap->Clear(1);

	if (normalMap != nullptr)
		normalMap->Clear(2);
}

void Material::SetShader(string Key)
{
	shader = Engine->AddShader(Key, Helper::stringTowstring(&Key));
}

void Material::SetDiffuseMap(string key)
{
	diffuseMap = Engine->FindTexture(key);
	DiffuseName = key;
}

void Material::SetDiffuseMap(string key, string Path)
{
	diffuseMap = Engine->AddTexture(key, Helper::stringTowstring(&Path));
	DiffuseName = Path;
}

void Material::SetSpecularMap(string key)
{
	specularMap = Engine->FindTexture(key);

	SpecularName = key;
}

void Material::SetSpecularMap(string key, string Path)
{
	specularMap = Engine->AddTexture(key, Helper::stringTowstring(&Path));
	SpecularName = Path;
}

void Material::SetNormalMap(string key)
{
	normalMap = Engine->FindTexture(key);

	NormalName = key;
}

void Material::SetNormalMap(string key, string Path)
{
	normalMap = Engine->AddTexture(key, Helper::stringTowstring(&Path));
	NormalName = Path;
}
