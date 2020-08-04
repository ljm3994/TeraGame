#include "Framework.h"
#include "Inventory.h"
#include <codecvt>

Inventory::Inventory(UINT Width, UINT Height)
	:Width(Width), Height(Height)
{
	BackgroundSprite = make_unique<DirectX::SpriteBatch>(Engine->GetContext());
	SlotSprite = make_unique<DirectX::SpriteBatch>(Engine->GetContext());
	IBSprite = make_unique<DirectX::SpriteBatch>(Engine->GetContext());
	MouseSlotSprite = make_unique<DirectX::SpriteBatch>(Engine->GetContext());
	RemovePopupSprite = make_unique<DirectX::SpriteBatch>(Engine->GetContext());
	PopupFontSprite = make_unique<DirectX::SpriteBatch>(Engine->GetContext());
	font = make_unique<DirectX::SpriteFont>(Engine->GetDevice(), L"Font/myfileb.spritefont");
	SelectX = -1;
	SelectY = -1;
	Inventransform = new Transform();
	Inventransform->SetPosition(100.0f, 100.0f, 100.0f);
	PopupPos = D3DXVECTOR2((Engine->GetClientWidth() / 2) - 100.0f, (Engine->GetClientHeight() / 2) - 100.0f);
	BackGroundTex = Engine->AddTexture("InventoryBackground", L"Texture/UI/InGameTableWindow_I2D.png");
	SlotDisableTex = Engine->AddTexture("InventorySlotDisable", L"Texture/UI/slotDisable.png");
	RemoveBtnTex = Engine->AddTexture("PopupBtnTex", L"Texture/UI/WindowBtn.png");
	RemoveBtnClickTex = Engine->AddTexture("PopupBtnClickTex", L"Texture/UI/WindowBtnClick.png");

	isShowRemovePopup = false;
	InvenIBTex = Engine->AddTexture("InvenIB", L"Texture/UI/PCInventory_IB.png");
	SlotsTex = Engine->AddTexture("InvenSlot", L"Texture/UI/slotFrame.png");
	SelectItem = nullptr;
	depth = new DepthStencilState();
	InvenSizeFactor = 0.7f;
	SetRect(&rcInven, 0, 0, 512, 1024);
	SetRect(&rcPopup, 0, 0, 512, 800);
	SetRect(&rcBtn, 0, 0, 150, 50);
	isInvenShow = false;
	isMouseDown = false;
	CreateInvenSlots();
}

Inventory::~Inventory()
{
	delete depth;
	delete Inventransform;

	for (UINT i = 0; i < Width; i++)
	{
		for (UINT j = 0; j < Height; j++)
			delete iteminfos[i][j];
	}
}

ItemInfo* Inventory::GetInventoryItem(UINT i, UINT j)
{
	return iteminfos[i][j];
}

void Inventory::CreateInvenSlots()
{
	iteminfos.resize(Width);

	for (UINT i = 0; i < iteminfos.size(); i++)
	{
		iteminfos[i].resize(Height);
	}
	D3DXVECTOR3 StartPos = Inventransform->GetPosition();
	for (UINT i = 0; i < Width; i++)
	{
		for (UINT j = 0; j < Height; j++)
		{
			ItemInfo* info = new ItemInfo();
			info->IconImg = nullptr;
			info->isEquipment = false;
			info->ItemID = -1;
			info->ItemName = "";
			info->type = ItemType::NONE;
			info->itemCount = 0;
			info->Pos.x = i * SlotSize;
			info->Pos.y = j * SlotSize;
			SetRect(&info->rcSlot, 0, 0, 64, 64);

			iteminfos[i][j] = info;
		}
	}
}

void Inventory::Update(float DeltaTime)
{
	if (isShowRemovePopup)
	{
		auto mstat = InputMouse->GetState();
		RECT rcOK, rcCancel;
		SetRect(&rcOK, rcBtn.left + PopupPos.x + 50.0f, rcBtn.bottom + PopupPos.y + 100.0f, rcBtn.right + PopupPos.x + 50.0f, rcBtn.top + PopupPos.y + 100.0f);
		SetRect(&rcCancel, rcBtn.left + PopupPos.x + 200.0f, rcBtn.bottom + PopupPos.y + 100.0f, rcBtn.right + PopupPos.x + 200.0f, rcBtn.top + PopupPos.y + 100.0f);

		if (mstat.IsleftButton)
		{
			if (Helper::PtInRect(rcOK, D3DXVECTOR2(mstat.x, mstat.y)))
			{
				SelectItem->IconImg = nullptr;
				SelectItem->isEquipment = false;
				SelectItem->itemCount = 0;
				SelectItem->ItemID = -1;
				SelectItem->ItemName = "";
				SelectItem->type = ItemType::NONE;

				isShowRemovePopup = false;

				SelectItem = nullptr;

				return;
			}

			if (Helper::PtInRect(rcCancel, D3DXVECTOR2(mstat.x, mstat.y)))
			{
				isShowRemovePopup = false;
				SelectItem = nullptr;

				return;
			}
		}
	}
}

bool Inventory::isInInventoryMouse(D3DXVECTOR2 MousePos)
{
	if (isInvenShow)
	{
		RECT rc;
		D3DXVECTOR3 StartPos = Inventransform->GetPosition();
		SetRect(&rc, rcInven.left + StartPos.x, rcInven.bottom + StartPos.y, rcInven.right + StartPos.x, rcInven.top + StartPos.y);

		return Helper::PtInRect(rc, MousePos);
	}
	else
		return false;
}

ItemInfo* Inventory::ItemUsed(bool isDown, D3DXVECTOR2 MousePos)
{
	if (isShowRemovePopup) return nullptr;

	if (isInvenShow)
	{
		if (isDown && !isMouseDown)
		{
			D3DXVECTOR3 StartPos = Inventransform->GetPosition();

			StartPos.x += 10.0f;
			StartPos.y += 40.0f;

			for (UINT i = 0; i < Width; i++)
			{
				for (UINT j = 0; j < Height; j++)
				{
					RECT rc;

					if (iteminfos[i][j]->IconImg && !iteminfos[i][j]->isEquipment)
					{
						SetRect(&rc, iteminfos[i][j]->rcSlot.left + ((StartPos.x) + i * (SlotSize)),
							iteminfos[i][j]->rcSlot.bottom + (StartPos.y) + j * (SlotSize),
							iteminfos[i][j]->rcSlot.right + ((StartPos.x) + i * (SlotSize)),
							iteminfos[i][j]->rcSlot.top + (StartPos.y) + j * (SlotSize));

						if (Helper::PtInRect(rc, D3DXVECTOR2(MousePos.x, MousePos.y)))
						{
							return iteminfos[i][j];
						}
					}
				}
			}
		}
	}
	return nullptr;
}

void Inventory::DragInven(bool isDown, D3DXVECTOR2 MousePos)
{
	if (isShowRemovePopup) return;

	D3DXVECTOR3 StartPos = Inventransform->GetPosition();

	StartPos.x += 10.0f;
	StartPos.y += 40.0f;

	if (!isMouseDown && isDown)       // 마우스 클릭시
	{
		isMouseDown = true;

		if (!SelectItem)
		{
			for (UINT i = 0; i < Width; i++)
			{
				for (UINT j = 0; j < Height; j++)
				{
					RECT rc;

					if (iteminfos[i][j]->IconImg && !iteminfos[i][j]->isEquipment)
					{
						SetRect(&rc, iteminfos[i][j]->rcSlot.left + ((StartPos.x) + i * (SlotSize)),
							iteminfos[i][j]->rcSlot.bottom + (StartPos.y) + j * (SlotSize),
							iteminfos[i][j]->rcSlot.right + ((StartPos.x) + i * (SlotSize)),
							iteminfos[i][j]->rcSlot.top + (StartPos.y) + j * (SlotSize));
						if (Helper::PtInRect(rc, D3DXVECTOR2(MousePos.x, MousePos.y)))
						{
							SelectX = i;
							SelectY = j;
							SelectItem = iteminfos[i][j];
						}
					}
				}
			}
		}
	}
	else if (isMouseDown && !isDown)     //드래그를 똇을떄경우
	{
		isMouseDown = false;

		if (SelectItem)
		{
			for (UINT i = 0; i < Width; i++)
			{
				for (UINT j = 0; j < Height; j++)
				{
					RECT rc;
					SetRect(&rc, iteminfos[i][j]->rcSlot.left + ((StartPos.x) + i * (SlotSize)),
						iteminfos[i][j]->rcSlot.bottom + (StartPos.y) + j * (SlotSize),
						iteminfos[i][j]->rcSlot.right + ((StartPos.x) + i * (SlotSize)),
						iteminfos[i][j]->rcSlot.top + (StartPos.y) + j * (SlotSize));

					if (Helper::PtInRect(rc, D3DXVECTOR2(MousePos.x, MousePos.y)))
					{
						if (iteminfos[i][j]->IconImg)
						{
							ItemInfo* temp = iteminfos[i][j];
							iteminfos[i][j] = iteminfos[SelectX][SelectY];
							iteminfos[SelectX][SelectY] = temp;

							SelectItem = nullptr;

							return;
						}
						else
						{
							if (isInInventoryMouse(D3DXVECTOR2(MousePos.x, MousePos.y)))
							{
								ItemInfo* temp = iteminfos[i][j];
								iteminfos[i][j] = iteminfos[SelectX][SelectY];
								iteminfos[SelectX][SelectY] = temp;

								SelectItem = nullptr;

								return;
							}
						}
					}
				}
			}

			if (!isInInventoryMouse(D3DXVECTOR2(MousePos.x, MousePos.y)))
			{
				isShowRemovePopup = true;
			}
		}
	}
}

void Inventory::Render()
{
	if (isInvenShow)
	{
		D3DXVECTOR3 StartPos = Inventransform->GetPosition();

		BackgroundSprite->Begin(DirectX::SpriteSortMode_Texture);

		BackgroundSprite->Draw(BackGroundTex->GetSrv(), DirectX::XMFLOAT2(StartPos.x, StartPos.y), &rcInven, DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(InvenSizeFactor, InvenSizeFactor), DirectX::SpriteEffects::SpriteEffects_None);

		BackgroundSprite->End();

		SlotSprite->Begin(DirectX::SpriteSortMode_Texture);

		StartPos.x += 10.0f;
		StartPos.y += 40.0f;
		for (UINT i = 0; i < Width; i++)
		{
			for (UINT j = 0; j < Height; j++)
			{
				SlotSprite->Draw(SlotsTex->GetSrv(), DirectX::XMFLOAT2((StartPos.x) + i * (SlotSize), (StartPos.y) + j * (SlotSize)),
					&iteminfos[i][j]->rcSlot, DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f),
					DirectX::SpriteEffects::SpriteEffects_None);

				if (iteminfos[i][j]->ItemID != -1)
				{
					IBSprite->Begin(DirectX::SpriteSortMode_Texture);

					IBSprite->Draw(iteminfos[i][j]->IconImg->GetSrv(), DirectX::XMFLOAT2((StartPos.x) + i * (SlotSize), (StartPos.y) + j * (SlotSize)),
						&iteminfos[i][j]->rcSlot, DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(0.8f, 0.8f),
						DirectX::SpriteEffects::SpriteEffects_None);

					IBSprite->End();

					if (iteminfos[i][j]->isEquipment)
					{
						IBSprite->Begin(DirectX::SpriteSortMode_Texture);

						IBSprite->Draw(SlotDisableTex->GetSrv(), DirectX::XMFLOAT2((StartPos.x) + i * (SlotSize), (StartPos.y) + j * (SlotSize)),
							&iteminfos[i][j]->rcSlot, DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f),
							DirectX::SpriteEffects::SpriteEffects_None);

						IBSprite->End();
					}
				}
			}
		}

		SlotSprite->End();

		auto mstate = InputMouse->GetState();

		if (!isShowRemovePopup)
		{
			if (isMouseDown)
			{
				if (SelectItem)
				{
					MouseSlotSprite->Begin(DirectX::SpriteSortMode_Texture);

					MouseSlotSprite->Draw(SelectItem->IconImg->GetSrv(), DirectX::XMFLOAT2(mstate.x, mstate.y), &SelectItem->rcSlot, DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(0.8f, 0.8f),
						DirectX::SpriteEffects::SpriteEffects_None);

					MouseSlotSprite->End();
				}
			}
		}
		else
		{
			if (SelectItem)
			{
				BackgroundSprite->Begin(DirectX::SpriteSortMode_Texture);

				BackgroundSprite->Draw(BackGroundTex->GetSrv(), DirectX::XMFLOAT2(PopupPos.x, PopupPos.y), &rcPopup, DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(0.8f, 0.2f), DirectX::SpriteEffects::SpriteEffects_None);

				BackgroundSprite->End();

				RemovePopupSprite->Begin();

				RemovePopupSprite->Draw(RemoveBtnTex->GetSrv(), DirectX::XMFLOAT2(PopupPos.x + 50.0f, PopupPos.y + 100.0f), &rcBtn, DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(0.8f, 0.8f), DirectX::SpriteEffects::SpriteEffects_None);

				RemovePopupSprite->Draw(RemoveBtnTex->GetSrv(), DirectX::XMFLOAT2(PopupPos.x + 200.0f, PopupPos.y + 100.0f), &rcBtn, DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(0.8f, 0.8f), DirectX::SpriteEffects::SpriteEffects_None);

				RemovePopupSprite->End();

				PopupFontSprite->Begin();
				
				font->DrawString(PopupFontSprite.get(), L"Item Remove?", DirectX::XMFLOAT2(PopupPos.x + 50.0f, PopupPos.y + 20.0f), DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f));

				font->DrawString(PopupFontSprite.get(), L"OK", DirectX::XMFLOAT2(PopupPos.x + 90.0f, PopupPos.y + 100.0f), DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(0.6f, 0.5f));

				font->DrawString(PopupFontSprite.get(), L"CANCEL", DirectX::XMFLOAT2(PopupPos.x + 220.0f, PopupPos.y + 100.0f), DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(0.6f, 0.5f));

				PopupFontSprite->End();
			}
		}
		depth->Set();
	}
}

void Inventory::SetInvenItem(UINT i, UINT j, string ImageKey, UINT itemID, string ItemName, ItemType type, UINT itemCount, bool isEquipment)
{
	if (Width > i && Height > j)
	{
		iteminfos[i][j]->IconImg = Engine->FindTexture(ImageKey);
		iteminfos[i][j]->ItemID = itemID;
		iteminfos[i][j]->ItemName = ItemName;
		iteminfos[i][j]->type = type;
		iteminfos[i][j]->itemCount = itemCount;
		iteminfos[i][j]->isEquipment = isEquipment;
	}
}
