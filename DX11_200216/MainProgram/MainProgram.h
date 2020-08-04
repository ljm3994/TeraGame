#pragma once

class MainProgram
{
public:
	MainProgram();
	~MainProgram();

	void Init();
	void Update(ID3D11RenderTargetView * backbuffer, UINT ClienWidth, UINT ClientHeight, float delta, float TotalTime);

	void PreRender();
	void Render();
	void PostRender();
	void SetImgui();
};