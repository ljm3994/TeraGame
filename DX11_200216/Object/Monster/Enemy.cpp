#include "Framework.h"
#include "Enemy.h"
#include "FrameWork\Collision\BoxCollision.h"
#include <Object\Charter.h>

Enemy::Enemy(string MonsterKey)
{
	model = Engine->GetComponentModel<SkinningModel>(MonsterKey);
	AttkDelayTime = 0.0f;
	CurrentState = new EnemyState[model->GetInstanceCount()];
	PrevState = new EnemyState[model->GetInstanceCount()];
	DeatWait = new float[model->GetInstanceCount()];
	for (UINT i = 0; i < model->GetInstanceCount(); i++)
	{
		Transform* temp = new Transform();
		isAtk.emplace_back(false);
		Enemydeath.emplace_back(false);
		model->AddBoneTransform("Bip01-Prop1", i, temp);
		WeaponTrans.emplace_back(temp);
		D3DXVECTOR3 Velo = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		Velocity.emplace_back(Velo);
		DeatWait[i] = 0.0f;
		CurrentState[i] = EnemyState::ENEMY_WAIT;
		PrevState[i] = EnemyState::ENEMY_NONE;
	}
}

Enemy::~Enemy()
{
	delete[] CurrentState;
	delete[] PrevState;
	delete[] InstanceHP;

	for (auto item : WeaponTrans)
	{
		delete item;
	}
}

void Enemy::Move(float DeltaTime)
{

	for (UINT i = 0; i < model->GetInstanceCount(); i++)
	{
		if (model->GetActive(i))
		{
			D3DXVECTOR3 currentPos = model->GetTransform(i)->GetPosition();
			D3DXVECTOR3 dirToTarget = PlayerTrans->GetPosition() - currentPos;

			float dist = D3DXVec3Length(&dirToTarget);

			D3DXVec3Normalize(&Velocity[i], &dirToTarget);
			Vector3 zero = Vector3(0, 0, 0);
			D3DXVec3Lerp(&Velocity[i], &Velocity[i], &zero, 0.2);

			if (CurrentState[i] != Enemy::EnemyState::ENEMY_DEATH)
			{
				if (dist <= 20.0f && dist > 4.0f)
				{
					if (CurrentState[i] != Enemy::EnemyState::ENEMY_RUN && !isAtk[i])
					{
						CurrentState[i] = Enemy::EnemyState::ENEMY_RUN;
						isAtk[i] = false;
					}

					currentPos += model->GetTransform(i)->GetRight() * MoveSpeed * DeltaTime;

					model->GetTransform(i)->SetPosition(currentPos.x, currentPos.y, currentPos.z);
					Rotation(i, DeltaTime);

					D3DXVECTOR3 outpos;
					currentPos.y += 1000.0;
					if (terrain->GetHeight(currentPos, outpos))
					{
						model->GetTransform(i)->SetPosition(outpos.x, outpos.y, outpos.z);
					}
				}
				else if (dist > 20.0f)
				{
					if (CurrentState[i] != Enemy::EnemyState::ENEMY_WAIT)
					{
						CurrentState[i] = Enemy::EnemyState::ENEMY_WAIT;
						isAtk[i] = false;
					}

					model->GetTransform(i)->SetYawAngle(0.0f);
				}
				else if (dist <= 4.1f && dist > 0.0f)
				{
					if (!isAtk[i])
					{
						isAtk[i] = true;
						CurrentState[i] = Enemy::EnemyState::ENEMY_ATTK1;
					}
					else
					{
						AttkDelayTime += DeltaTime;

						if (AttkDelayTime >= 4.0f)
						{
							AttkDelayTime = 0.0f;

							Ray ray;
							ray.position = WeaponTrans[i]->GetPosition();
							ray.direction = PlayerTrans->GetPosition() - WeaponTrans[i]->GetPosition();
							if (player->GetColl()->IsCollision(ray))
							{
								player->SetPlayerHP(4.0f);
								player->GetColl()->GetColor()->SetColor(D3DXCOLOR(1, 0, 0, 1));
							}
						}
					}
					Rotation(i, DeltaTime);
				}

				PlayAnimation(i);
			}
		}
	}
}

void Enemy::Rotation(UINT instanceID, float DeltaTime)
{
	float magnitude = D3DXVec3Length(&Velocity[instanceID]);

	if (magnitude < 0.1f)
		return;
	else if (magnitude > 1.0f)
		D3DXVec3Normalize(&Velocity[instanceID], &Velocity[instanceID]);

	D3DXVECTOR3 Start;

	D3DXVec3Normalize(&Start, &model->GetTransform(instanceID)->GetRight());

	D3DXVECTOR3 End;

	D3DXVec3Normalize(&End, &Velocity[instanceID]);

	float angle = acos(D3DXVec3Dot(&Start, &End));

	if (angle < 0.1f)
	{
		return;
	}
	
	D3DXVECTOR3 Cross;
	D3DXVec3Cross(&Cross, &Start, &End);
	float DeltaAngle = 0.0f;
	DeltaAngle += Cross.y * DeltaTime * RotationSpeed;

	model->GetTransform(instanceID)->SetYawAngle(DeltaAngle);
}

bool Enemy::GetMonsterActive(UINT instanceID)
{
	return model->GetActive(instanceID);
}

bool Enemy::GetEnemyDeath(UINT InstanceID)
{
	return Enemydeath[InstanceID];
}

void Enemy::SetMonsterActive(UINT instanceID, bool val)
{
	model->SetActive(instanceID, val);
}

Transform* Enemy::GetMonsterPosition(UINT instanceID)
{
	return model->GetTransform(instanceID);
}

void Enemy::SetPlayerTransform(Transform * player)
{
	PlayerTrans = player;
}

void Enemy::SetInstanceHP(UINT instanceID, float HP)
{
	this->InstanceHP[instanceID] -= HP;

	if (this->InstanceHP[instanceID] <= 0.0f)
	{
		Enemydeath[instanceID] = true;
		CurrentState[instanceID] = Enemy::EnemyState::ENEMY_DEATH;
		isAtk[instanceID] = false;
		AttkDelayTime = 0.0f;

		PlayAnimation(instanceID);
	}
}

void Enemy::SetCurrentHp(UINT instanceID)
{
	this->Hp = InstanceHP[instanceID];
}

void Enemy::BaseObjectEnable()
{
}

void Enemy::BaseObjectDisable()
{
	
}

void Enemy::PlayAnimation(UINT instanceID)
{
	if (PrevState[instanceID] == CurrentState[instanceID])
		return;

	int Index = 0;
	ANI_STATE State = ANI_STATE::ANI_LOOP;
	
	switch (CurrentState[instanceID])
	{
	case Enemy::EnemyState::ENEMY_WAIT:
		Index =	(int)Enemy::EnemyState::ENEMY_WAIT;
		State = ANI_STATE::ANI_LOOP;
		break;
	case Enemy::EnemyState::ENEMY_RUN:
		Index = (int)Enemy::EnemyState::ENEMY_RUN;
		State = ANI_STATE::ANI_LOOP;
		break;
	case Enemy::EnemyState::ENEMY_ATTK1:
		Index = (int)Enemy::EnemyState::ENEMY_ATTK1;
		State = ANI_STATE::ANI_ONCE;
		break;
	case Enemy::EnemyState::ENEMY_ATTK2:
		Index = (int)Enemy::EnemyState::ENEMY_ATTK2;
		State = ANI_STATE::ANI_ONCE;
		break;
	case Enemy::EnemyState::ENEMY_DEATH:
		Index = (int)Enemy::EnemyState::ENEMY_DEATH;
		State = ANI_STATE::ANI_ONCE;
		break;
	}

	if (model->Play(Index, instanceID, State, 0.2f))
	{
		PrevState[instanceID] = CurrentState[instanceID];
	}
}

void Enemy::AnimationEvent(bool result, int Index, UINT instanceID)
{
	if (Index == (int)Enemy::EnemyState::ENEMY_ATTK1)
	{
		CurrentState[instanceID] = Enemy::EnemyState::ENEMY_RUN;
		isAtk[instanceID] = false;
		AttkDelayTime = 0.0f;
		PlayAnimation(instanceID);
	}
	else if (Index == (int)Enemy::EnemyState::ENEMY_DEATH)
	{
		model->SetActive(instanceID, false);
		//model->GetTransform(instanceID)->SetYawAngle(0.0f);
	}
}

void Enemy::BaseObjectUpdate(float timeDelta)
{
}

void Enemy::BaseObjectNoActiveUpdate(float timeDelte)
{
}

void Enemy::BaseObjectRender()
{
}

void Enemy::BaseObjectPostRender()
{
}

void Enemy::BaseObjectRenderShadow()
{
}
