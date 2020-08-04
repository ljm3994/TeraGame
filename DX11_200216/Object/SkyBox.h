#pragma once
#include <FrameWork\Render\Model\StaticModel.h>
#include "FrameWork\States\DepthStencilState.h"
#include "FrameWork\States\BlendState.h"
#include "FrameWork\States\RasterizerState.h"
#include "FrameWork\Buffer\Base\ConstantBuffer.h"

#define CUBEMAPSIZE 256
class SkyBox
{
	struct CloudeBuffer
	{
		D3DXVECTOR4 parameter0;
		D3DXVECTOR4 parameter1;
	};

	struct SelectEnvBuffer
	{
		int isEnv = false;
		float padding[3];
	};
	StaticModel* skymodel;
	StaticModel* CloudeModel;

	Shader* shader;
	Shader* psEnvironmentShader;
	Shader* psShaderCIEClearSky;
	Shader* psCloudeShader[2];

	Texture* DiffuseTex;
	Texture* FallOffTex;
	Texture* CloudeTex[2];
	Texture* TexPerlin;
	Texture* Normals;

	Texture* CubeMap;

	DepthStencilState* SkyDepth;
	BlendState* blendstate[2];
	RasterizeState* resterizestate;

	ConstantBuffer<CloudeBuffer> CloudeBufferConst;
	ConstantBuffer< SelectEnvBuffer> SelectBuffer;
	CloudeBuffer CloudeData[3];
public:
	SkyBox();
	~SkyBox();
	void Update(float DeltaTime);
	void Render(bool RenderCloude, bool EnvMap, bool CIEClearSky);

	void SetTextureSphere(string Path);
	void SetTextureCubeMap(string Path);

	ID3D11ShaderResourceView* GetCubeMapSRV() { return CubeMap->GetSrv(); }
	ID3D11ShaderResourceView* GetSphereSRV() { return DiffuseTex->GetSrv(); }
	
	const StaticModel* GetSkyModel() const { return skymodel; }

	void DrawSkySphere();
private:
	void SettingState();
};