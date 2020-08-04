#pragma once
#include "FrameWork\Render\PostProcess\PostProcess.h"

class FXAA : public PostProcess
{
private:
	Shader* shaderPS;
public:
	FXAA();
	~FXAA();

	virtual void Init();
	virtual void Render(ID3D11ShaderResourceView* input, ID3D11RenderTargetView* output);
	virtual void PostRender(UINT width, UINT height);
};