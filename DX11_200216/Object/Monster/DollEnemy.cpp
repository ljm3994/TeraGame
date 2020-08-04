#include "Framework.h"
#include "DollEnemy.h"

DollEnemy::DollEnemy()
	: Enemy("Doll")
{
	MoveSpeed = 10.0f;
	RotationSpeed = 5.f;
	model->AddAnimation("Monster/DollWait");
	model->AddAnimation("Monster/DollRun");
	model->AddAnimation("Monster/DollAttk1");
	model->AddAnimation("Monster/DollAttk2");
	model->AddAnimation("Monster/DollDeath");

	InstanceHP = new float[model->GetInstanceCount()];

	MaxHp = 100.0f;
	for (UINT i = 0; i < model->GetInstanceCount(); i++)
	{
		InstanceHP[i] = MaxHp;
	}
	auto func = bind(&Enemy::AnimationEvent, this, placeholders::_1, placeholders::_2, placeholders::_3);
	model->SetAnimationEvent(func);
}

DollEnemy::~DollEnemy()
{
	
}

void DollEnemy::BaseObjectUpdate(float timeDelta)
{
	Move(timeDelta);

	model->Update(timeDelta);
}

void DollEnemy::BaseObjectNoActiveUpdate(float timeDelte)
{
}

void DollEnemy::BaseObjectRender()
{
	model->Render();
}

void DollEnemy::BaseObjectRenderShadow()
{
}
