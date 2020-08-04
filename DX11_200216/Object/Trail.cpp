#include "Framework.h"
#include "Trail.h"

Trail::Trail(int Count)
	: Count(Count), DeltaTime(0.0f), ChangedTime(0.0001f), isBone(false), isCull(false), isDepth(false), isFirst(false)
{
	bonebuffer = new TrailBoneBuffer();
	colorbuffer = new TrailColorBuffer();
	bonebuffer->data.Count = Count;

	texture = Engine->AddTexture("TrailTex", L"Texture/Effect/E_Swordtrail005_emis.png");

	shader = Engine->AddShader("TrailShader", L"Trail");

	CreateData();

	CullMode[0] = new RasterizeState();
	CullMode[1] = new RasterizeState();
	CullMode[1]->CullMode(D3D11_CULL_NONE);

	DepthMode[0] = new DepthStencilState();
	DepthMode[1] = new DepthStencilState();
	DepthMode[1]->DepthEnable(false);

	BlendMode[0] = new BlendState();
	BlendMode[1] = new BlendState();
	BlendMode[1]->Alpha(true);
	BlendMode[1]->DestBlend(D3D11_BLEND_ONE);
	BlendMode[1]->SrcBlend(D3D11_BLEND_SRC_ALPHA);
	BlendMode[1]->BlendOP(D3D11_BLEND_OP_ADD);

	D3DXMatrixIdentity(&matrix);
}

Trail::~Trail()
{
	delete CullMode[0];
	delete CullMode[1];
	delete DepthMode[0];
	delete DepthMode[1];
	delete BlendMode[0];
	delete BlendMode[1];

	delete vertexbuffer;
	delete bonebuffer;
	delete colorbuffer;
}

void Trail::SetMatrix()
{
	if (!isFirst)
	{
		isFirst = true;

		if (isBone)
		{
			// 추후 만들것 일단 무기 이펙트만 만들거.....
		}
		else
		{
			matrix = *target->GetTransform(0)->GetWorld();
			matrix = (*transform->GetWorld()) * matrix;
		}

		D3DXMatrixTranspose(&matrix, &matrix);

		for (UINT i = 0; i < Count; i++)
		{
			bonebuffer->data.World[i] = matrix;
		}

		return;
	}

	for (UINT i = Count - 1; i > 0; i--)
	{
		bonebuffer->data.World[i] = bonebuffer->data.World[i - 1];
	}

	if (isBone)
	{

	}
	else
	{
		matrix = *target->GetTransform(0)->GetWorld();
		matrix = (*transform->GetWorld()) * matrix;
	}

	D3DXMatrixTranspose(&matrix, &matrix);
	bonebuffer->data.World[0] = matrix;
}

void Trail::CreateData()
{
	{
		VertexUV* vertices = new VertexUV[6 * Count];

		float x = 1.0f / Count;

		for (UINT i = 0; i < Count; i++)
		{
			vertices[i * 6 + 0].Position = D3DXVECTOR3(-0.5f + (x * i), -0.5f, 0.0f);
			vertices[i * 6 + 1].Position = D3DXVECTOR3(-0.5f + (x * i), 0.5f, 0.0f);
			vertices[i * 6 + 2].Position = D3DXVECTOR3(-0.5f + x * (i + 1), -0.5f, 0.0f);
			vertices[i * 6 + 3].Position = D3DXVECTOR3(-0.5f + x * (i + 1), -0.5f, 0.0f);
			vertices[i * 6 + 4].Position = D3DXVECTOR3(-0.5f + (x * i), 0.5f, 0.0f);
			vertices[i * 6 + 5].Position = D3DXVECTOR3(-0.5f + x * (i + 1), 0.5f, 0.0f);

			vertices[i * 6 + 0].UV = D3DXVECTOR2((x * i), 1);
			vertices[i * 6 + 1].UV = D3DXVECTOR2((x * i), 0);
			vertices[i * 6 + 2].UV = D3DXVECTOR2(x * (i + 1), 1);
			vertices[i * 6 + 3].UV = D3DXVECTOR2(x * (i + 1), 1);
			vertices[i * 6 + 4].UV = D3DXVECTOR2((x * i), 0);
			vertices[i * 6 + 5].UV = D3DXVECTOR2(x * (i + 1), 0);
		}

		vertexbuffer = new VertexBuffer(vertices, sizeof(VertexUV), 6 * Count);

		delete[] vertices;
	}
}

void Trail::BaseObjectEnable()
{
}

void Trail::BaseObjectDisable()
{
}

void Trail::BaseObjectUpdate(float timeDelta)
{
	DeltaTime += timeDelta;

	if (DeltaTime >= ChangedTime)
	{
		DeltaTime = 0;
		SetMatrix();
	}
}

void Trail::BaseObjectNoActiveUpdate(float timeDelte)
{
}

void Trail::BaseObjectRender()
{
	bonebuffer->SetVSBuffer(10);
	colorbuffer->SetPSBuffer(10);
	texture->PSSet(0);

	BlendMode[1]->Set();

	if (isDepth)
		DepthMode[1]->Set();
	if (isCull)
		CullMode[1]->Set();

	shader->SetRender(vertexbuffer);

	Engine->GetContext()->Draw(6 * Count, 0);

	CullMode[0]->Set();
	DepthMode[0]->Set();
	BlendMode[0]->Set();
}

void Trail::BaseObjectRenderShadow()
{
}

void Trail::SetTarget(string ModelKey, string BoneName)
{
	target = Engine->GetComponentModel<ModelInstance>(ModelKey);

	if (BoneName != "")
	{
		this->boneName = BoneName;
		isBone = true;
	}
}

void Trail::BaseObjectPostRender()
{
}
