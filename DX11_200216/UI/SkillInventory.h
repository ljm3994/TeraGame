#pragma once
#include <FrameWork\States\DepthStencilState.h>

struct SkillInfo
{
	UINT SkillID;
	string SkillName;
	Texture* SkillIconTex;
	RECT rcSlot;
	D3DXVECTOR2 Pos;
	float CoolTime;
	float CurrentCool;
	bool isUsed;
};
class SkillInventory
{
	unique_ptr<DirectX::SpriteBatch> SlotSprite;
	unique_ptr<DirectX::SpriteBatch> FontSprite;
	unique_ptr<DirectX::SpriteBatch> IBSprite;
	unique_ptr<DirectX::SpriteFont> font;

	Texture* InvenIBTex;
	Texture* SlotsTex;

	vector< SkillInfo*> SkillInfos;

	UINT MaxCount;
	DepthStencilState* depth;
public:
	SkillInventory();
	~SkillInventory();

	void Update(float DeltaTime);
	void Render();

	SkillInfo* UsedSkill(UINT index);
	void SetSkill(UINT i, string ImageKey, UINT itemID, string ItemName, float CollTime);
};