#pragma once
#include <FrameWork\Render\Model\ObjectBase.h>
#include <FrameWork\Render\Model\SkinningModel.h>
#include <Map\Terrain.h>
class Charter;

class Enemy : public ObjectBase
{
protected:
	enum class EnemyState : int
	{
		ENEMY_WAIT,
		ENEMY_RUN,
		ENEMY_ATTK1,
		ENEMY_ATTK2,
		ENEMY_DEATH,
		ENEMY_NONE
	};

	EnemyState* CurrentState;
	EnemyState* PrevState;
	vector<D3DXVECTOR3> Velocity;
	SkinningModel * model;
	Charter* player;
	Transform * PlayerTrans;
	float AttkDelayTime;
	float* DeatWait;
	vector<bool> Enemydeath;
	float MoveSpeed;
	float RotationSpeed;
	float * InstanceHP;
	float Hp;
	vector<bool> isAtk;
	vector<Transform*> WeaponTrans;
	float MaxHp;
	Terrain* terrain;
public:
	Enemy(string MonsterKey);
	virtual ~Enemy();
	void Move(float DeltaTime);
	void Rotation(UINT instanceID, float DeltaTime);
	bool GetMonsterActive(UINT instanceID);
	bool GetEnemyDeath(UINT InstanceID);
	void SetMonsterActive(UINT instanceID, bool val);
	Transform* GetMonsterPosition(UINT instanceID);
	void SetPlayerTransform(Transform * player);
	void SetTerrain(Terrain* val) { terrain = val; }
	void SetPlayerColl(Charter* coll) { player = coll; }
	void SetInstanceHP(UINT instanceID, float HP);
	void SetCurrentHp(UINT instanceID);
	float GetMonsterMaxHP() 
	{
		return MaxHp;
	}
	float GetMonsterCurrentHP()
	{
		return Hp;
	}
	SkinningModel* GetModel() { return model; }
	// ObjectBase을(를) 통해 상속됨
	virtual void BaseObjectEnable() override;
	virtual void BaseObjectDisable() override;
	virtual void PlayAnimation(UINT instanceID);
	virtual void AnimationEvent(bool result, int Index, UINT instanceID);

	// ObjectBase을(를) 통해 상속됨
	virtual void BaseObjectUpdate(float timeDelta) override;
	virtual void BaseObjectNoActiveUpdate(float timeDelte) override;
	virtual void BaseObjectRender() override;
	virtual void BaseObjectPostRender() override;
	virtual void BaseObjectRenderShadow() override;
};