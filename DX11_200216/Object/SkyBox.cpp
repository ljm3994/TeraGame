#include "Framework.h"
#include "SkyBox.h"

SkyBox::SkyBox()
{
	CloudeBufferConst.CreateBuffer();
	SelectBuffer.CreateBuffer();

	shader = Engine->AddShader("SkyBox", L"SkyBox");
	psCloudeShader[0] = Engine->AddShader("CloudeSkyBox1", L"SkyBox", ShaderCreateFlag::SHADER_FLAG_PS, "", "Cloude1");
	psCloudeShader[1] = Engine->AddShader("CloudeSkyBox2", L"SkyBox", ShaderCreateFlag::SHADER_FLAG_PS, "", "Cloude2");

	psShaderCIEClearSky = Engine->AddShader("CIEClearSky", L"CIEClearSky", ShaderCreateFlag::SHADER_FLAG_PS);

	Engine->AddModel("Sky", "Sky/Sky", MODELTYPE::STATIC_MODELTYPE, false, 1);
	Engine->AddModel("Cloude", "Sky/Cloude", MODELTYPE::STATIC_MODELTYPE, false, 1);

	skymodel = Engine->GetComponentModel<StaticModel>("Sky");
	CloudeModel = Engine->GetComponentModel<StaticModel>("Cloude");

	FallOffTex = Engine->AddTexture("SkyFallOff", L"Texture/Environment/skybox_falloff.dds", false, true);
	TexPerlin = Engine->AddTexture("SkyPerlin", L"Texture/Environment/clouds_normal.dds", false, true);

	CloudeTex[0] = Engine->AddTexture("Cloude1", L"Texture/Environment/clouds_normal.dds", false, true);
	CloudeTex[1] = Engine->AddTexture("Cloude2", L"Texture/Environment/w3_clouds.dds", false, true);

	Normals = Engine->AddTexture("SkyNormal", L"Texture/Environment/w3_clouds_normal.dds", false, true);

	SettingState();
}

SkyBox::~SkyBox()
{
	delete SkyDepth;
	delete blendstate[0];
	delete blendstate[1];
	delete resterizestate;
}

void SkyBox::Update(float DeltaTime)
{
	D3DXVECTOR3 CamPos = Engine->GetCurrentCamera()->GetEye();
	skymodel->GetTransform(0)->SetPosition(CamPos.x, CamPos.y, CamPos.z);
	skymodel->GetTransform(0)->SetScale(100.0f, 100.0f, 100.0f);
	skymodel->Update(DeltaTime);

	CloudeModel->GetTransform(0)->SetPosition(CamPos.x, CamPos.y - 300.0f, CamPos.z);
	CloudeModel->GetTransform(0)->SetScale(1.f, 1.f, 1.f);
	CloudeModel->Update(DeltaTime);
}

void SkyBox::SettingState()
{
	SkyDepth = new DepthStencilState();

	SkyDepth->StencilEnable(false);
	SkyDepth->DepthEnable(true);
	SkyDepth->DepthFunc(D3D11_COMPARISON_EQUAL);
	SkyDepth->DepthWirteMask(D3D11_DEPTH_WRITE_MASK_ZERO);

	blendstate[0] = new BlendState();
	blendstate[1] = new BlendState();
	blendstate[0]->RenderTargetSet(true, D3D11_BLEND_ONE, D3D11_BLEND_INV_SRC_ALPHA, D3D11_BLEND_OP_ADD,
		D3D11_BLEND_OP_ADD, D3D11_BLEND_ONE, D3D11_BLEND_ZERO, D3D11_COLOR_WRITE_ENABLE_ALL);

	resterizestate = new RasterizeState();

	resterizestate->CullMode(D3D11_CULL_FRONT);
}


void SkyBox::Render(bool RenderCloude, bool EnvMap, bool CIEClearSky)
{
	UINT meshCount = skymodel->GetMeshCount();
	shader->SetVS(true);

	if(CubeMap)
		CubeMap->PSSet(0);

	TexPerlin->PSSet(4);
	Normals->PSSet(5);

	if(DiffuseTex != nullptr)
		DiffuseTex->PSSet(1);

	FallOffTex->PSSet(2);

	SkyDepth->Set();

	if (CIEClearSky)
	{
		psShaderCIEClearSky->SetPS();
	}
	else
	{
		SelectBuffer.GetData().isEnv = EnvMap;
		SelectBuffer.SetPSBuffer(11);
	}

	skymodel->GetTransform(0)->SetVS();

	for (UINT i = 0; i < meshCount; i++)
	{
		auto mesh = skymodel->GetMeshData(i);
		mesh->vertexbuffers->VSSet();
		mesh->indexbuffers->VSSet();
		
		Engine->GetContext()->DrawIndexed(mesh->indices.size(), 0, 0);
	}

	if (RenderCloude)
	{
		D3DXVECTOR4 BlendFactor = { 1, 1, 1, 1 };

		for (UINT index = 1; index < 2; ++index)
		{
			CloudeBufferConst.GetData() = CloudeData[index];
			CloudeBufferConst.SetPSBuffer(10);

			CloudeTex[index]->PSSet(0);

			blendstate[0]->Set(BlendFactor);

			psCloudeShader[index]->SetPS();
			CloudeModel->GetTransform(0)->SetVS();

			UINT meshCount = CloudeModel->GetMeshCount();
			for (UINT i = 0; i < meshCount; i++)
			{
				auto mesh = CloudeModel->GetMeshData(i);

				mesh->vertexbuffers->VSSet();
				mesh->indexbuffers->VSSet();

				const UINT drawCalls = (index == 0) ? 1 : 2;

				for (UINT j = 0; j < drawCalls; j++)
				{
					Engine->GetContext()->DrawIndexed(mesh->indices.size(), 0, 0);
				}
			}
		}

		Engine->GetContext()->OMSetBlendState(0, D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f), 0xffffffff);
	}
}

void SkyBox::SetTextureSphere(string Path)
{
	DiffuseTex = Engine->AddTexture(Path, Helper::stringTowstring(&Path));
}

void SkyBox::SetTextureCubeMap(string Path)
{
	CubeMap = Engine->AddTexture(Path, Helper::stringTowstring(&Path));
}

void SkyBox::DrawSkySphere()
{
	skymodel->GetTransform(0)->SetVS();

	UINT meshCount = skymodel->GetMeshCount();

	for (UINT i = 0; i < meshCount; i++)
	{
		auto mesh = skymodel->GetMeshData(i);
		mesh->vertexbuffers->VSSet();
		mesh->indexbuffers->VSSet();

		Engine->GetContext()->DrawIndexed(mesh->indices.size(), 0, 0);
	}
}
