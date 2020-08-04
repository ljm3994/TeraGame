#include "Framework.h"
#include "SkillInventory.h"

SkillInventory::SkillInventory()
{
	InvenIBTex = Engine->AddTexture("InvenIB", L"Texture/UI/PCInventory_IB.png");
	SlotsTex = Engine->AddTexture("InvenSlot", L"Texture/UI/slotFrame.png");

	SlotSprite = make_unique<DirectX::SpriteBatch>(Engine->GetContext());
	FontSprite = make_unique<DirectX::SpriteBatch>(Engine->GetContext());
	IBSprite = make_unique<DirectX::SpriteBatch>(Engine->GetContext());

	font = make_unique<DirectX::SpriteFont>(Engine->GetDevice(), L"Font/myfileb.spritefont");

	MaxCount = 6;

	SkillInfos.resize(MaxCount);

	for (UINT i = 0; i < MaxCount; i++)
	{
		SkillInfos[i] = new SkillInfo();

		SkillInfos[i]->CoolTime = 0.0f;
		SkillInfos[i]->CurrentCool = 0.0f;
		SkillInfos[i]->SkillIconTex = nullptr;
		SkillInfos[i]->isUsed = false;
		SkillInfos[i]->SkillName = "";
		SkillInfos[i]->SkillID = -1;
		SkillInfos[i]->Pos = D3DXVECTOR2(((Engine->GetClientWidth() / 2) - 500) + ((i * 64) + 400),
			Engine->GetClientHeight() - 160);
		SetRect(&SkillInfos[i]->rcSlot, 0, 0, 64, 64);
	}

	depth = new DepthStencilState();
}

SkillInventory::~SkillInventory()
{
	delete depth;

	for (UINT i = 0; i < MaxCount; i++)
	{
		delete SkillInfos[i];
	}
}

void SkillInventory::Update(float DeltaTime)
{
	for (UINT i = 0; i < MaxCount; i++)
	{
		if (SkillInfos[i]->isUsed)
		{
			if (SkillInfos[i]->CurrentCool > 0.0f)
			{
				SkillInfos[i]->CurrentCool -= DeltaTime;
			}
			else
			{
				SkillInfos[i]->isUsed = false;
			}
		}
	}
}

void SkillInventory::Render()
{
	SlotSprite->Begin(DirectX::SpriteSortMode_Texture);

	for (UINT i = 0; i < MaxCount; i++)
	{
		SlotSprite->Draw(SlotsTex->GetSrv(), DirectX::XMFLOAT2(SkillInfos[i]->Pos.x, SkillInfos[i]->Pos.y), &SkillInfos[i]->rcSlot
			, DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f));

		if (SkillInfos[i]->SkillID != -1)
		{
			IBSprite->Begin();

			IBSprite->Draw(SkillInfos[i]->SkillIconTex->GetSrv(), DirectX::XMFLOAT2(SkillInfos[i]->Pos.x, SkillInfos[i]->Pos.y),
				&SkillInfos[i]->rcSlot, DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(0.8f, 0.8f));

			IBSprite->End();

			if (SkillInfos[i]->isUsed)
			{
				RECT orginal = SkillInfos[i]->rcSlot;
				float Width = orginal.right * (SkillInfos[i]->CurrentCool / SkillInfos[i]->CoolTime);

				orginal.right = Width;
				IBSprite->Begin();

				IBSprite->Draw(InvenIBTex->GetSrv(), DirectX::XMFLOAT2(SkillInfos[i]->Pos.x, SkillInfos[i]->Pos.y),
					&orginal, DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f));

				IBSprite->End();

				FontSprite->Begin();

				char * output = new char[128];
				sprintf_s(output, 128, "%.2f", SkillInfos[i]->CurrentCool);

				font->DrawString(FontSprite.get(), output, DirectX::XMFLOAT2(SkillInfos[i]->Pos.x, SkillInfos[i]->Pos.y),
					DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(0.4f, 0.4f));

				FontSprite->End();

				delete[] output;
			}
		}
	}

	SlotSprite->End();

	depth->Set();
}

SkillInfo* SkillInventory::UsedSkill(UINT index)
{
	if (!SkillInfos[index]->isUsed && SkillInfos[index]->SkillID != -1)
	{
		SkillInfos[index]->isUsed = true;
		SkillInfos[index]->CurrentCool = SkillInfos[index]->CoolTime;

		return SkillInfos[index];
	}

	return nullptr;
}

void SkillInventory::SetSkill(UINT i, string ImageKey, UINT itemID, string ItemName, float CollTime)
{
	SkillInfos[i]->SkillIconTex = Engine->FindTexture(ImageKey);
	SkillInfos[i]->SkillID = itemID;
	SkillInfos[i]->SkillName = ItemName;
	SkillInfos[i]->CurrentCool = 0.0f;
	SkillInfos[i]->CoolTime = CollTime;
	SkillInfos[i]->isUsed = false;
}
