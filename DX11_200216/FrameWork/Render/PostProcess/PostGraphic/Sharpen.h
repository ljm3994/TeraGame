#pragma once
#include "FrameWork\Buffer\Base\ConstantBuffer.h"
class FullScreenPass;

class Sharpen
{
private:
	struct Data
	{
		D3DXVECTOR4 Parameter1;
		D3DXVECTOR4 Parameter2;

		Data()
		{
			Parameter1 = D3DXVECTOR4(2.0f, 1.76f, 0.025f, -0.25f);
			Parameter2 = D3DXVECTOR4(-10.0f, 0.15285f, 0.0f, 1.0f);
		}
	};

	Shader* shader;
	ConstantBuffer<Data> sharpenbuffer;

	FullScreenPass* fullscreenpass;
public:
	Sharpen();
	~Sharpen();

	void Init(FullScreenPass* fullscreenpass);
	void Render(ID3D11ShaderResourceView* Inputs[], UINT InputsCount, ID3D11RenderTargetView* Output);
};