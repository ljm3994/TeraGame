#include "Framework.h"
#include "Sky.h"

Sky::Sky(string SkyFilePath)
{
	skymodel = Engine->GetComponentModel<StaticModel>(SkyFilePath);

	//skymodel->GetTransform(0)->SetScaleFactor(0.001f);
	for (UINT i = 0; i < skymodel->GetMeshCount(); i++)
	{
		skymodel->GetMeshData(0)->vertices[i].Position *= 0.001f;
	}
}

Sky::~Sky()
{

}

void Sky::Update(float DeltaTime)
{
	D3DXVECTOR3 CamPos = Engine->GetCurrentCamera()->GetEye();
	skymodel->GetTransform(0)->SetPosition(CamPos.x, CamPos.y, CamPos.z);

	skymodel->Update(DeltaTime);
}

void Sky::Render()
{
	skymodel->Render();
}
