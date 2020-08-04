#pragma once
#include <FrameWork\Render\Model\ObjectBase.h>
#include "Object\Trail.h"
#include <Object\Monster\Enemy.h>
#include <UI\Inventory.h>
#include <UI\SkillInventory.h>

class SkinningModel;

class Charter : public ObjectBase
{
private:
	enum CHARACTERMESH_PART {
		Part_BODY = 0,
		Part_FACE = 1,
		Part_HAIR = 2,
		Part_TAIL = 3,
		Part_RWEAPON = 4,
		Part_LWEAPON = 5
	};

	enum tagState
	{
		State_Wait = 0,
		State_Walk = 1,
		State_InWeapon = 2,
		State_OutWeapon = 3,
		State_Dash,
		State_Combo1 = 6,
		State_Combo1R = 7,
		State_Combo2 = 8,
		State_Combo2R = 9,
		State_Combo3 = 10,
		State_Combo3R = 11,
		State_Combo4 = 12,
		State_Rapid1 = 13,
		State_Rapid2 = 14,
		State_Rapid2R = 15,
		State_Rapid3 = 16,
		State_None
	};

	ItemInfo* CurrentBodyEquipment;
	vector<ModelInstance*> objects;
	map<string, SkinningModel*> bodymeshmap;
	Keyboard::KeyboardEvent keyEvent;
	bool isAttack;
	bool isAttackReady;
	float AttackDelay;
	bool isDash;
	float MouseClickDelay;
	tagState state;
	tagState PrevState;
	D3DXVECTOR3 Velocity;
	float WaitDelayTime;
	float RotationSpeed;
	float MoveSpeed;
	float PlayerHp;
	float PlayerMp;
	float PlayerMaxHp;
	float PlayerMaxMp;
	bool isEnemyColl;
	Enemy* enemy;
	Trail* trail[2];
	Inventory* inven;
	SkillInventory* skillinven;
public:
	Charter();
	~Charter();

	bool CharterInit(string Body, string Face, string Hair, string Tail, string RWeapon, string LWeapon);
	void Move(float DeltaTime);
	float GetPlayerHp() { return PlayerHp; }
	float GetPlayMaxHp() { return PlayerMaxHp; }
	float GetPlayerMp() { return PlayerMp; }
	float GetPlayMaxMp() { return PlayerMaxMp; }
	void SetPlayerHP(float Hp);
	BoxCollision* GetColl() { return objects[0]->GetColl(0); }
	BoxCollision* GetLColl() { return objects[Part_LWEAPON]->GetColl(0); }
	BoxCollision* GetRColl() { return objects[Part_RWEAPON]->GetColl(0); }
	void SetEnemy(Enemy* enem) { enemy = enem; }
	bool GetIsAttack() { return isAttack; }
	bool GetIsEnemyColl() { return isEnemyColl; }
	// ObjectBase을(를) 통해 상속됨
	virtual void BaseObjectEnable() override;
	virtual void BaseObjectDisable() override;
	virtual void BaseObjectUpdate(float timeDelta) override;
	virtual void BaseObjectNoActiveUpdate(float timeDelte) override;
	virtual void BaseObjectRender() override;
	virtual void BaseObjectRenderShadow() override;
	virtual void BaseObjectPostRender() override;
private:
	void EquipmentItem(ItemInfo* item);
	void AddBodyAnimation(SkinningModel* bodymesh, string BodyName);
	void AttackAnimation(bool isBtnDown, float DeltaTime);
	void AnimationPlay(tagState state);
	void AnimationEvent(bool Result, int Index, UINT instanceID);
	void AddAnimation(SkinningModel* facemesh, SkinningModel* tailmesh, SkinningModel* hairmesh);
};