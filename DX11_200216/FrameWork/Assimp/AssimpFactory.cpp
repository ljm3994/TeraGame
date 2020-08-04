#include "Framework.h"
#include "AssimpFactory.h"
#include "FrameWork\Utility\Xml.h"
#include "FrameWork\Utility\BinaryHelper.h"
#include <assimp/cimport.h>
#include <FrameWork\Assimp\MapExporter.h>

SkinnedMesh* AssimpFactory::ExtractSkinnedMesh(string FileName, string SaveFileName, bool IsOverWrite, aiPostProcessSteps AiParameter)
{
	Assimp::Importer* importer;
	const aiScene* scene;

	importer = new Assimp::Importer();

	if (AiParameter == 0)
	{
		scene = importer->ReadFile(FileName, aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_MaxQuality);
	}
	else
	{
		scene = importer->ReadFile(FileName, AiParameter);
	}

	assert(scene != nullptr);

	SkinnedMesh* model = new SkinnedMesh(scene, L"Model");
	model->WriteMaterialFile(DefaultMaterialPath + SaveFileName + ".mat", IsOverWrite);
	model->WriteMeshFile(DefaultMeshPath + SaveFileName + ".mesh", IsOverWrite);
	delete importer;

	return model;
}

void AssimpFactory::AddAnimation(SkinnedMesh* mesh, string FileName, string SaveFileName, bool IsOverWrite, bool isRootLock, aiPostProcessSteps AiParameter)
{
	Assimp::Importer* importer;
	const aiScene* scene;

	importer = new Assimp::Importer();

	if (AiParameter == 0)
	{
		scene = importer->ReadFile(FileName, aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_MaxQuality
			| aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace | aiProcess_ImproveCacheLocality | aiProcess_FindDegenerates |
			aiProcess_FindInvalidData | aiProcess_FixInfacingNormals | aiProcess_JoinIdenticalVertices);
	}
	else
	{
		scene = importer->ReadFile(FileName, AiParameter);
	}

	assert(scene != nullptr);

	mesh->ReadAnimation(scene, isRootLock);
	mesh->WriteAnimationFile(AnimationDefaultPath + SaveFileName + ".anim", IsOverWrite);
	delete importer;
}

void AssimpFactory::MapDataExporter(string MapModelPath, string saveFileName, bool IsOverWrite, aiPostProcessSteps AiParameter)
{
	Assimp::Importer* importer;
	const aiScene* scene;

	importer = new Assimp::Importer();

	if (AiParameter == 0)
	{
		scene = importer->ReadFile(MapModelPath, aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_MaxQuality);
	}
	else
	{
		scene = importer->ReadFile(MapModelPath, AiParameter);
	}

	assert(scene != nullptr);

	MapExporter* map = new MapExporter(scene);
	map->WriteMaterial(DefaultMaterialPath + saveFileName + ".mat", IsOverWrite);
	map->WriteMesh(DefaultMeshPath + saveFileName + ".mesh", IsOverWrite);

	delete map;
	delete importer;
}
