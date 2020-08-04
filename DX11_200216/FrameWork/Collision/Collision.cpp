#include "Framework.h"
#include "Collision.h"

Collision::Collision()
{
	shader = Engine->AddShader(L"Collision");
	color = new TrailColorBuffer();
	color->SetColor(D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f));
}

Collision::~Collision()
{
	delete vertexbuffer;
	delete indexbuffer;
	delete color;
}

void Collision::Update(float DeltaTime)
{
	UpdateWorld();
}

void Collision::Render()
{
	SetVS();
	color->SetPSBuffer(10);
	shader->SetRender(vertexbuffer, indexbuffer, D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	Engine->GetContext()->DrawIndexed(indexCount, 0, 0);
}
