#pragma once
#include "FrameWork\Buffer\Base\ConstantBuffer.h"
class FullScreenPass
{
private:
	Shader* shader;
	Shader* RecopyShader;
	ConstantBuffer<D3DXVECTOR4> buffer;

public:
	FullScreenPass();
	~FullScreenPass();

	void Create();

	void RenderTri();
	void Render(UINT width, UINT height);
	void Render(UINT targetWidth, UINT targetHeight, ID3D11ShaderResourceView* InSourceSRV, ID3D11RenderTargetView* OutTargetRTV, Shader* PixelShader);
	void RenderTriEx(ID3D11RenderTargetView* OutRTV, ID3D11ShaderResourceView* InSRV, Shader* OverridePS);
	void RenderFullScreenTriRecopy(ID3D11RenderTargetView* OutRTV, ID3D11ShaderResourceView* InSRV);

	void RenderScaling(UINT targetWidth, UINT targetHeight, ID3D11ShaderResourceView* InSourceSRV, ID3D11RenderTargetView* OutTargetRTV);

};