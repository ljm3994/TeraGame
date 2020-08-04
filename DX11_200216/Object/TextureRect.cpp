#include "Framework.h"
#include "TextureRect.h"

TextureRect::TextureRect()
{
	VertexUV vertices[4];
	vertices[0].Position = D3DXVECTOR3(-0.5f, 0.5f, 0.0f);
	vertices[1].Position = D3DXVECTOR3(0.5f, 0.5f, 0.0f);
	vertices[2].Position = D3DXVECTOR3(-0.5f, -0.5f, 0.0f);
	vertices[3].Position = D3DXVECTOR3(0.5f, -0.5f, 0.0f);

	vertices[0].UV = D3DXVECTOR2(0, 0);
	vertices[1].UV = D3DXVECTOR2(1, 0);
	vertices[2].UV = D3DXVECTOR2(0, 1);
	vertices[3].UV = D3DXVECTOR2(1, 1);

	vertexbuffer = new VertexBuffer(vertices, sizeof(VertexUV), 4);

	UINT indices[] = {
		0, 1, 2,
		1, 3, 2
	};

	indicesbuffer = new IndexBuffer(indices, 6);

	texture = Engine->AddTexture("Texture0", L"Texture/LoadingImage/LoadingImage2_Tex.png");
	shader = Engine->AddShader("Texture", L"Texture");
}

TextureRect::~TextureRect()
{
	delete vertexbuffer;
	delete indicesbuffer;
}

void TextureRect::Update()
{
	UpdateWorld();
}

void TextureRect::Render()
{
	SetVS();
	texture->PSSet(0);

	shader->SetRender(vertexbuffer, indicesbuffer);

	Engine->GetContext()->DrawIndexed(6, 0, 0);
}

void TextureRect::ChangeTexture(string Tag)
{
	texture = Engine->FindTexture(Tag);
}
