#pragma once
#include "FrameWork\DXTypes\DepthStencilBuffer.h"
#include "FrameWork\DXTypes\RenderTarget2D.h"

class GBuffer
{
public:
	DepthStencilBuffer depthstencilbuffer;
	RenderTarget2D RenderTarget0;
	RenderTarget2D RenderTarget1;
	RenderTarget2D RenderTarget2;
	RenderTarget2D RenderTarget3;

	void Init(UINT width, UINT height);
	void Set();
	void ClearGBuffer(float DepthClearValue, bool bClearGBuffer);

	void Release();
};