#pragma once
#include <FrameWork\DXTypes\RenderTarget2D.h>

class FullScreenPass;

class LensFlares
{
private:
	Shader* PSShaderThresHold;
	Shader* PSShaderGhost;
	Shader* PSShaderBlurH;
	Shader* PSShaderBlurV;

	FullScreenPass* fullscreenpass;

public:
	RenderTarget2D renderTargetHalfRes;
	RenderTarget2D renderTargetHalfRes2;
	LensFlares();
	~LensFlares();

	void Init(FullScreenPass* pFullscreenPass, UINT FullscreenWidth, UINT FullscreenHeight);
	void Render(ID3D11ShaderResourceView* InputSRV);

	void Release();
};