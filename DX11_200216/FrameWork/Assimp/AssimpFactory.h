#pragma once
#include "AssimpHelper.h"
#include <FrameWork\Assimp\SkinnedMesh.h>

class AssimpFactory
{
public:
	static SkinnedMesh* ExtractSkinnedMesh(string FileNamer, string SaveFileName, bool IsOverWrite = false, aiPostProcessSteps AiParameter = (aiPostProcessSteps)0);
	static void AddAnimation(SkinnedMesh* mesh, string FileName, string SaveFileName, bool IsOverWrite = false, bool isRootLock = false, aiPostProcessSteps AiParameter = (aiPostProcessSteps)0);
	static void MapDataExporter(string MapModelPath, string saveFileName, bool IsOverWrite = false, aiPostProcessSteps AiParameter = (aiPostProcessSteps)0);
};