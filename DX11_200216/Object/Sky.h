#pragma once
#include <Object\Sphere.h>
#include "FrameWork\States\RasterizerState.h"
#include "FrameWork\States\DepthStencilState.h"
#include <FrameWork\Render\Model\StaticModel.h>

class Sky
{
private:
	StaticModel* skymodel;
public:
	Sky(string SkyFilePath);
	~Sky();

	void Update(float DeltaTime);
	void Render();
};