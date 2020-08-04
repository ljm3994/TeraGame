#pragma once
#include "FrameWork\States\DepthStencilState.h"

#define SlotSize 64


enum class ItemType
{
	NONE,
	WeaponType,
	ClothType,
	UseType
};

struct ItemInfo
{
	UINT ItemID;
	RECT rcSlot;
	D3DXVECTOR2 Pos;
	string ItemName;
	ItemType type;
	Texture* IconImg;
	UINT itemCount;
	bool isEquipment;
};

class Inventory
{
	bool isInvenShow;
	bool isMouseDown;
	bool isShowRemovePopup;
	float InvenSizeFactor;
	RECT rcInven;
	RECT rcPopup;
	RECT rcBtn;
	unique_ptr<DirectX::SpriteBatch> BackgroundSprite;
	unique_ptr<DirectX::SpriteBatch> IBSprite;
	unique_ptr<DirectX::SpriteBatch> SlotSprite;
	unique_ptr<DirectX::SpriteBatch> MouseSlotSprite;
	unique_ptr<DirectX::SpriteBatch> RemovePopupSprite;
	unique_ptr<DirectX::SpriteBatch> PopupFontSprite;
	unique_ptr<DirectX::SpriteFont> font;
	Texture* BackGroundTex;
	Texture* RemoveBtnTex;
	Texture* RemoveBtnClickTex;
	Texture* InvenIBTex;
	Texture* SlotsTex;
	Texture* SlotDisableTex;
	vector<vector<ItemInfo*>> iteminfos;
	ItemInfo* SelectItem;
	DepthStencilState* depth;
	Transform* Inventransform;
	D3DXVECTOR2 PopupPos;
	UINT SelectX;
	UINT SelectY;
	UINT Width;
	UINT Height;
public:
	Inventory(UINT Width = 5, UINT Height = 6);
	~Inventory();
	ItemInfo* GetInventoryItem(UINT i, UINT j);
	ItemInfo* GetSelectItem() { return SelectItem; };
	void CreateInvenSlots();
	void Update(float DeltaTime);
	bool isInInventoryMouse(D3DXVECTOR2 MousePos);
	ItemInfo* ItemUsed(bool isDown, D3DXVECTOR2 MousePos);
	void DragInven(bool isDown, D3DXVECTOR2 MousePos);
	void SetInvenShow() { isInvenShow = !isInvenShow; }
	bool GetisInvenShow() { return isInvenShow; }
	bool GetisPopupShow() { return isShowRemovePopup; }
	void Render();
	void SetInvenItem(UINT i, UINT j, string ImageKey, UINT itemID, string ItemName, ItemType type, UINT itemCount = 1, bool isEquipment = false);
};