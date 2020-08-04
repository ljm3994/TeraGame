#include "Framework.h"
#include "Charter.h"
#include <FrameWork\Render\Model/StaticModel.h>
#include <FrameWork\Render\Model\SkinningModel.h>
#include "FrameWork\Collision\BoxCollision.h"

Charter::Charter()
	: RotationSpeed(5.0f), state(State_Wait), isAttack(false), PlayerHp(100), PlayerMaxHp(100), MoveSpeed(4.5f),
	Velocity(0.f, 0.f, 0.f), isDash(false), isAttackReady(false), WaitDelayTime(0.0f), PrevState(State_None),
	PlayerMaxMp(100), PlayerMp(100), CurrentBodyEquipment(nullptr)
{
	isEnemyColl = false;
	MouseClickDelay = 0.0f;
}

Charter::~Charter()
{
	if(trail[0])
		delete trail[0];

	if(trail[1])
		delete trail[1];

	delete inven;
	delete skillinven;
}

bool Charter::CharterInit(string Body, string Face, string Hair, string Tail, string RWeaponKey, string LWeaponKey)
{
	inven = new Inventory();
	skillinven = new SkillInventory();

	skillinven->SetSkill(0, "Rapid1", 0, "Rapid1", 10.0f);
	skillinven->SetSkill(1, "Rapid2", 1, "Rapid2", 15.0f);
	skillinven->SetSkill(2, "Rapid3", 2, "Rapid3", 20.0f);

	inven->SetInvenItem(0, 0, "Event11", 0, "Body11", ItemType::ClothType);
	inven->SetInvenItem(1, 0, "Event17", 0, "Body17", ItemType::ClothType);
	inven->SetInvenItem(2, 0, "Event21", 0, "Body21", ItemType::ClothType);
	inven->SetInvenItem(3, 0, "Event25", 0, "Body25", ItemType::ClothType);
	inven->SetInvenItem(4, 0, "Event80", 0, "Body", ItemType::ClothType, 1, true);

	CurrentBodyEquipment = inven->GetInventoryItem(4, 0);
	SkinningModel* bodymesh = Engine->GetComponentModel<SkinningModel>(Body);
	SkinningModel* bodymesh2 = Engine->GetComponentModel<SkinningModel>("Body11");
	SkinningModel* bodymesh3 = Engine->GetComponentModel<SkinningModel>("Body17");
	SkinningModel* bodymesh4 = Engine->GetComponentModel<SkinningModel>("Body21");
	SkinningModel* bodymesh5 = Engine->GetComponentModel<SkinningModel>("Body25");

	SkinningModel* FaceMesy = Engine->GetComponentModel<SkinningModel>(Face);
	SkinningModel* HairMesy = Engine->GetComponentModel<SkinningModel>(Hair);
	SkinningModel* TailMesy = Engine->GetComponentModel<SkinningModel>(Tail);
	StaticModel* RWeapon = Engine->GetComponentModel<StaticModel>(RWeaponKey);
	StaticModel* LWeapon = Engine->GetComponentModel<StaticModel>(LWeaponKey);
	AttackDelay = 0.0f;
	if (!bodymesh || !FaceMesy || !HairMesy)  //인간적으로 몸, 얼굴, 헤어는 있어야겠다........
		return false;

	auto func = bind(&Charter::AnimationEvent, this, placeholders::_1, placeholders::_2, placeholders::_3);
	bodymesh->AddBoneTransform("Bip01-Neck", 0, FaceMesy->GetTransform(0));

	FaceMesy->AddBoneTransform("Dummy_Face", 0, HairMesy->GetTransform(0));

	bodymesh->GetTransform(0)->SetParent(transform->GetWorld());

	HairMesy->GetTransform(0)->SetParent(FaceMesy->GetTransform(0)->GetWorld());
	FaceMesy->GetTransform(0)->SetParent(bodymesh->GetTransform(0)->GetWorld());

	objects.emplace_back(bodymesh);
	objects.emplace_back(FaceMesy);
	objects.emplace_back(HairMesy);

	if (TailMesy)
	{
		bodymesh->AddBoneTransform("Bip01-Spine", 0, TailMesy->GetTransform(0));

		TailMesy->GetTransform(0)->SetParent(bodymesh->GetTransform(0)->GetWorld());

		objects.emplace_back(TailMesy);
	}

	if (RWeapon)
	{
		//Weapon_Back 대기중 등에 무기
		// R_Sword // 오른쪽 무기
		bodymesh->AddBoneTransform("Weapon_Back", 0, RWeapon->GetTransform(0));

		RWeapon->GetTransform(0)->SetParent(bodymesh->GetTransform(0)->GetWorld());
		RWeapon->GetTransform(0)->SetPitchAngle(-45.0f * (D3DX_PI / 180.0f));
		RWeapon->GetTransform(0)->SetScaleFactor(0.6f);
		objects.emplace_back(RWeapon);
		trail[0] = new Trail();
		trail[0]->GetTransform()->SetScale(10.0f, 10.0f, 1.0f);
		trail[0]->GetTransform()->SetPosition(20.0f, 0.0f, 0.0f);
		trail[0]->SetTarget(RWeaponKey);
	}
	if (LWeapon)
	{
		bodymesh->AddBoneTransform("Weapon_Back", 0, LWeapon->GetTransform(0));
		LWeapon->GetTransform(0)->SetParent(bodymesh->GetTransform(0)->GetWorld());
		LWeapon->GetTransform(0)->SetPitchAngle(-60.0f * (D3DX_PI / 180.0f));
		LWeapon->GetTransform(0)->SetScaleFactor(0.6f);
		objects.emplace_back(LWeapon);
		trail[1] = new Trail();
		trail[1]->GetTransform()->SetScale(10.0f, 10.0f, 1.0f);
		trail[1]->GetTransform()->SetPosition(20.0f, 0.0f, 0.0f);
		trail[1]->SetTarget(LWeaponKey);
	}

	AddAnimation(FaceMesy, TailMesy, HairMesy);

	AddBodyAnimation(bodymesh, "Elin80");
	AddBodyAnimation(bodymesh2, "Elin11");
	AddBodyAnimation(bodymesh3, "Elin17");
	AddBodyAnimation(bodymesh4, "Elin21");
	AddBodyAnimation(bodymesh5, "Elin25");

	bodymeshmap.insert(make_pair("Body", bodymesh));
	bodymeshmap.insert(make_pair("Body11", bodymesh2));
	bodymeshmap.insert(make_pair("Body17", bodymesh3));
	bodymeshmap.insert(make_pair("Body21", bodymesh4));
	bodymeshmap.insert(make_pair("Body25", bodymesh5));

	bodymesh->SetAnimationEvent(func);
	bodymesh2->SetAnimationEvent(func);
	FaceMesy->SetAnimationEvent(func);
	HairMesy->SetAnimationEvent(func);
	TailMesy->SetAnimationEvent(func);
	return true;
}

void Charter::Move(float DeltaTime)
{
	float magnitude = D3DXVec3Length(&Velocity);

	if (magnitude > 1.0f)
		D3DXVec3Normalize(&Velocity, &Velocity);

	if (magnitude > 0.1f)
	{
		D3DXVECTOR3 pos = transform->GetPosition();
		pos += Velocity * MoveSpeed * DeltaTime;

		if (!isAttack && isDash)
		{
			state = State_Dash;
		}
		else if (!isAttack && !isDash)
		{
			state = State_Walk;
		}

		Vector3 zero = Vector3(0, 0, 0);

		D3DXVec3Lerp(&Velocity, &Velocity, &zero, MoveSpeed * DeltaTime);

		transform->SetPosition(pos.x, pos.y, pos.z);
	}
	else
	{
		if (state == State_Dash || state == State_Walk)
		{
			state = State_Wait;
		}
	}
}

void Charter::SetPlayerHP(float Hp)
{
	PlayerHp -= Hp;
}

void Charter::BaseObjectEnable()
{
}

void Charter::BaseObjectDisable()
{
}

void Charter::BaseObjectUpdate(float timeDelta)
{
	auto kstate = InputKey->GetState();
	auto mstate = InputMouse->GetState();
	keyEvent.Update(kstate);

	if (!isAttack)
	{
		float DeltaAngle = 0.0f;
		if (kstate.A)
		{
			WaitDelayTime = 0.0f;
			DeltaAngle -= 180.0f * 0.017453f * timeDelta;
		}
		if (kstate.D)
		{
			WaitDelayTime = 0.0f;
			DeltaAngle = 180.0f * 0.017453f * timeDelta;
		}
		transform->SetYawAngle(DeltaAngle);
		if (kstate.W)
		{
			WaitDelayTime = 0.0f;
			Velocity += transform->GetRight() * MoveSpeed * timeDelta;
		}
		if (keyEvent.pressed.NumPad1)
		{
			auto temp = skillinven->UsedSkill(0);

			if (temp)
			{
				state = tagState::State_Rapid1;

				trail[0]->SetActive(true);
				trail[1]->SetActive(true);
			}
		}
		else if (keyEvent.pressed.NumPad2)
		{
			auto temp = skillinven->UsedSkill(1);

			if (temp)
			{
				state = tagState::State_Rapid2;

				trail[0]->SetActive(true);
				trail[1]->SetActive(true);
			}
		}
		else if (keyEvent.pressed.NumPad3)
		{
			auto temp = skillinven->UsedSkill(2);

			if (temp)
			{
				state = tagState::State_Rapid3;

				trail[0]->SetActive(true);
				trail[1]->SetActive(true);
			}
		}
	}

	if (keyEvent.pressed.I)
	{
		inven->SetInvenShow();
	}
	
	if (!inven->GetisPopupShow())
	{
		if (!inven->isInInventoryMouse(D3DXVECTOR2(mstate.x, mstate.y)) && !inven->GetSelectItem())
		{
			AttackAnimation(mstate.IsleftButton, timeDelta);
		}
		else
		{
			inven->DragInven(mstate.IsleftButton, D3DXVECTOR2(mstate.x, mstate.y));
			ItemInfo* item = inven->ItemUsed(mstate.IsrightButton, D3DXVECTOR2(mstate.x, mstate.y));

			if (item)
			{
				EquipmentItem(item);
			}
		}
	}

	if (isAttack)
	{
		if (trail[0])
			trail[0]->Update(timeDelta);
		if (trail[1])
			trail[1]->Update(timeDelta);
		D3DXVECTOR3 pos = transform->GetPosition();
		Velocity += transform->GetRight() * 5.0f * timeDelta;
	}

	if (isAttackReady)
	{
		WaitDelayTime += timeDelta;

		if (WaitDelayTime > 10.0f)
		{
			WaitDelayTime = 0.0f;
			state = Charter::State_InWeapon;
		}
	}

	Move(timeDelta);
	AnimationPlay(state);

	inven->Update(timeDelta);
	skillinven->Update(timeDelta);

	for (auto item : objects)
		item->Update(timeDelta);
}

void Charter::BaseObjectNoActiveUpdate(float timeDelte)
{
}

void Charter::BaseObjectRender()
{
	for (auto item : objects)
		item->Render();

	if (trail[0])
		trail[0]->Render();
	if (trail[1])
		trail[1]->Render();
}

void Charter::BaseObjectRenderShadow()
{
}

void Charter::AddAnimation(SkinningModel* facemesh, SkinningModel* tailmesh, SkinningModel* hairmesh)
{
	//얼굴 애니메이션 추가
	facemesh->AddAnimation("Elin80/ElinFaceIdle");
	facemesh->AddAnimation("Elin80/ElinFaceWalk");
	facemesh->AddAnimation("Elin80/ElinDualWeaponAnim/ElinFaceWeaponInWeapon");
	facemesh->AddAnimation("Elin80/ElinDualWeaponAnim/ElinFaceWeaponOutWeapon");
	facemesh->AddAnimation("Elin80/ElinDualWeaponAnim/ElinFaceWeaponWait");
	facemesh->AddAnimation("Elin80/ElinDualWeaponAnim/ElinFaceWeaponRun");
	facemesh->AddAnimation("Elin80/ElinDualWeaponAnim/ElinFaceWeaponCombo1");
	facemesh->AddAnimation("Elin80/ElinDualWeaponAnim/ElinFaceWeaponCombo1R");
	facemesh->AddAnimation("Elin80/ElinDualWeaponAnim/ElinFaceWeaponCombo2");
	facemesh->AddAnimation("Elin80/ElinDualWeaponAnim/ElinFaceWeaponCombo2R");
	facemesh->AddAnimation("Elin80/ElinDualWeaponAnim/ElinFaceWeaponCombo3");
	facemesh->AddAnimation("Elin80/ElinDualWeaponAnim/ElinFaceWeaponCombo3R");
	facemesh->AddAnimation("Elin80/ElinDualWeaponAnim/ElinFaceWeaponCombo4");
	facemesh->AddAnimation("Elin80/ElinDualWeaponAnim/ElinFaceWeaponRapid1");
	facemesh->AddAnimation("Elin80/ElinDualWeaponAnim/ElinFaceWeaponRapid2");
	facemesh->AddAnimation("Elin80/ElinDualWeaponAnim/ElinFaceWeaponRapid2R");
	facemesh->AddAnimation("Elin80/ElinDualWeaponAnim/ElinFaceWeaponRapid3");

	//헤어 애니메이션 추가
	hairmesh->AddAnimation("Elin80/ElinHairIdle");
	hairmesh->AddAnimation("Elin80/ElinHairWalk");
	hairmesh->AddAnimation("Elin80/ElinDualWeaponAnim/ElinHairWeaponInWeapon");
	hairmesh->AddAnimation("Elin80/ElinDualWeaponAnim/ElinHairWeaponOutWeapon");
	hairmesh->AddAnimation("Elin80/ElinDualWeaponAnim/ElinHairWeaponWait");
	hairmesh->AddAnimation("Elin80/ElinDualWeaponAnim/ElinHairWeaponRun");
	hairmesh->AddAnimation("Elin80/ElinDualWeaponAnim/ElinHairWeaponCombo1");
	hairmesh->AddAnimation("Elin80/ElinDualWeaponAnim/ElinHairWeaponCombo1R");
	hairmesh->AddAnimation("Elin80/ElinDualWeaponAnim/ElinHairWeaponCombo2");
	hairmesh->AddAnimation("Elin80/ElinDualWeaponAnim/ElinHairWeaponCombo2R");
	hairmesh->AddAnimation("Elin80/ElinDualWeaponAnim/ElinHairWeaponCombo3");
	hairmesh->AddAnimation("Elin80/ElinDualWeaponAnim/ElinHairWeaponCombo3R");
	hairmesh->AddAnimation("Elin80/ElinDualWeaponAnim/ElinHairWeaponCombo4");
	hairmesh->AddAnimation("Elin80/ElinDualWeaponAnim/ElinHairWeaponRapid1");
	hairmesh->AddAnimation("Elin80/ElinDualWeaponAnim/ElinHairWeaponRapid2");
	hairmesh->AddAnimation("Elin80/ElinDualWeaponAnim/ElinHairWeaponRapid2R");
	hairmesh->AddAnimation("Elin80/ElinDualWeaponAnim/ElinHairWeaponRapid3");

	// 꼬리 애니메이션 추가
	if (tailmesh)
	{
		tailmesh->AddAnimation("Elin80/ElinTailIdle");
		tailmesh->AddAnimation("Elin80/ElinTailWalk");
		tailmesh->AddAnimation("Elin80/ElinDualWeaponAnim/ElinTailWeaponInWeapon");
		tailmesh->AddAnimation("Elin80/ElinDualWeaponAnim/ElinTailWeaponOutWeapon");
		tailmesh->AddAnimation("Elin80/ElinDualWeaponAnim/ElinTailWeaponWait");
		tailmesh->AddAnimation("Elin80/ElinDualWeaponAnim/ElinTailWeaponRun");
		tailmesh->AddAnimation("Elin80/ElinDualWeaponAnim/ElinTailWeaponCombo1");
		tailmesh->AddAnimation("Elin80/ElinDualWeaponAnim/ElinTailWeaponCombo1R");
		tailmesh->AddAnimation("Elin80/ElinDualWeaponAnim/ElinTailWeaponCombo2");
		tailmesh->AddAnimation("Elin80/ElinDualWeaponAnim/ElinTailWeaponCombo2R");
		tailmesh->AddAnimation("Elin80/ElinDualWeaponAnim/ElinTailWeaponCombo3");
		tailmesh->AddAnimation("Elin80/ElinDualWeaponAnim/ElinTailWeaponCombo3R");
		tailmesh->AddAnimation("Elin80/ElinDualWeaponAnim/ElinTailWeaponCombo4");
		tailmesh->AddAnimation("Elin80/ElinDualWeaponAnim/ElinTailWeaponRapid1");
		tailmesh->AddAnimation("Elin80/ElinDualWeaponAnim/ElinTailWeaponRapid2");
		tailmesh->AddAnimation("Elin80/ElinDualWeaponAnim/ElinTailWeaponRapid2R");
		tailmesh->AddAnimation("Elin80/ElinDualWeaponAnim/ElinTailWeaponRapid3");
	}
}

void Charter::BaseObjectPostRender()
{
	skillinven->Render();
	inven->Render();
}

void Charter::EquipmentItem(ItemInfo* item)
{
	if (item->type == ItemType::ClothType)
	{
		if (CurrentBodyEquipment)
		{
			CurrentBodyEquipment->isEquipment = false;
		}

		item->isEquipment = true;
		CurrentBodyEquipment = item;
		((SkinningModel*)objects[Part_BODY])->AllClearBoneTransform(0);
		((SkinningModel*)objects[Part_BODY])->GetTransform(0)->SetParent(nullptr);

		SkinningModel* model = bodymeshmap[item->ItemName];

		model->AddBoneTransform("Bip01-Neck", 0, objects[Part_FACE]->GetTransform(0));
		model->AddBoneTransform("Bip01-Spine", 0, objects[Part_TAIL]->GetTransform(0));
		model->AddBoneTransform("Weapon_Back", 0, objects[Part_LWEAPON]->GetTransform(0));
		model->AddBoneTransform("Weapon_Back", 0, objects[Part_RWEAPON]->GetTransform(0));
		model->GetTransform(0)->SetParent(transform->GetWorld());

		objects[Part_FACE]->GetTransform(0)->SetParent(model->GetTransform(0)->GetWorld());
		objects[Part_TAIL]->GetTransform(0)->SetParent(model->GetTransform(0)->GetWorld());
		objects[Part_LWEAPON]->GetTransform(0)->SetParent(model->GetTransform(0)->GetWorld());
		objects[Part_RWEAPON]->GetTransform(0)->SetParent(model->GetTransform(0)->GetWorld());

		objects[Part_BODY] = model;

		PrevState = tagState::State_None;
		AnimationPlay(state);
	}
}

void Charter::AddBodyAnimation(SkinningModel* bodymesh, string BodyName)
{
	//몸통 애니메이션 추가
	bodymesh->AddAnimation(BodyName + "/ElinBodyIdle");
	bodymesh->AddAnimation(BodyName + "/ElinBodyWalk");
	bodymesh->AddAnimation(BodyName + "/ElinDualWeaponAnim/ElinBodyWeaponInWeapon");
	bodymesh->AddAnimation(BodyName + "/ElinDualWeaponAnim/ElinBodyWeaponOutWeapon");
	bodymesh->AddAnimation(BodyName + "/ElinDualWeaponAnim/ElinBodyWeaponWait");
	bodymesh->AddAnimation(BodyName + "/ElinDualWeaponAnim/ElinBodyWeaponRun");
	bodymesh->AddAnimation(BodyName + "/ElinDualWeaponAnim/ElinBodyWeaponCombo1");
	bodymesh->AddAnimation(BodyName + "/ElinDualWeaponAnim/ElinBodyWeaponCombo1R");
	bodymesh->AddAnimation(BodyName + "/ElinDualWeaponAnim/ElinBodyWeaponCombo2");
	bodymesh->AddAnimation(BodyName + "/ElinDualWeaponAnim/ElinBodyWeaponCombo2R");
	bodymesh->AddAnimation(BodyName + "/ElinDualWeaponAnim/ElinBodyWeaponCombo3");
	bodymesh->AddAnimation(BodyName + "/ElinDualWeaponAnim/ElinBodyWeaponCombo3R");
	bodymesh->AddAnimation(BodyName + "/ElinDualWeaponAnim/ElinBodyWeaponCombo4");
	bodymesh->AddAnimation(BodyName + "/ElinDualWeaponAnim/ElinBodyWeaponRapid1");
	bodymesh->AddAnimation(BodyName + "/ElinDualWeaponAnim/ElinBodyWeaponRapid2");
	bodymesh->AddAnimation(BodyName + "/ElinDualWeaponAnim/ElinBodyWeaponRapid2R");
	bodymesh->AddAnimation(BodyName + "/ElinDualWeaponAnim/ElinBodyWeaponRapid3");
}

void Charter::AttackAnimation(bool isBtnDown, float DeltaTime)
{
	if (isBtnDown)
	{
		WaitDelayTime = 0.0f;
		if (!isAttackReady)
		{
			state = Charter::State_OutWeapon;
		}
		else
		{
			if (!isAttack)
			{
				trail[0]->SetActive(true);
				trail[1]->SetActive(true);
				Engine->GetSoundSystem()->Play("AttkSound");
				state = Charter::State_Combo1;
				isAttack = true;
			}
			else
			{
				AttackDelay += DeltaTime;
				if (AttackDelay > 0.2f)
				{
					AttackDelay = 0.0f;
					isEnemyColl = false;
					UINT EnemyCount = enemy->GetModel()->GetInstanceCount();

					for (UINT i = 0; i < EnemyCount; i++)
					{
						if (enemy->GetMonsterActive(i) && !enemy->GetEnemyDeath(i))
						{
							BoxCollision* coll = enemy->GetModel()->GetColl(i);
							if (coll->IsCollision(GetLColl()) || coll->IsCollision(GetRColl()))
							{
								isEnemyColl = true;
								enemy->SetInstanceHP(i, 10.0f);
								enemy->SetCurrentHp(i);
								coll->GetColor()->SetColor(D3DXCOLOR(1, 0, 0, 1));
							}
						}
					}
				}
				if (state == Charter::State_Combo1R)
				{
					Engine->GetSoundSystem()->Play("AttkSound");
					state = Charter::State_Combo2;
				}
				else if (state == Charter::State_Combo2R)
				{
					Engine->GetSoundSystem()->Play("AttkSound");
					state = Charter::State_Combo3;
				}
				else if (state == Charter::State_Combo3R)
				{
					Engine->GetSoundSystem()->Play("AttkSound");
					state = Charter::State_Combo4;
				}
			}
		}
	}
}

void Charter::AnimationPlay(tagState state)
{
	if (PrevState == state) return;

	int Index = 0;
	ANI_STATE State = ANI_STATE::ANI_LOOP;

	switch (state)
	{
	case Charter::State_Combo1:
		Index = State_Combo1;
		State = ANI_STATE::ANI_ONCE;
		isAttack = true;
		break;
	case Charter::State_Combo1R:
		Index = State_Combo1R;
		State = ANI_STATE::ANI_ONCE;
		isAttack = true;
		break;
	case Charter::State_Combo2:
		Index = State_Combo2;
		State = ANI_STATE::ANI_ONCE;
		isAttack = true;
		break;
	case Charter::State_Combo2R:
		Index = State_Combo2R;
		State = ANI_STATE::ANI_ONCE;
		isAttack = true;
		break;
	case Charter::State_Combo3:
		Index = State_Combo3;
		State = ANI_STATE::ANI_ONCE;
		isAttack = true;
		break;
	case Charter::State_Combo3R:
		Index = State_Combo3R;
		State = ANI_STATE::ANI_ONCE;
		isAttack = true;
		break;
	case Charter::State_Combo4:
		Index = State_Combo4;
		State = ANI_STATE::ANI_ONCE;
		isAttack = true;
		break;
	case Charter::State_Dash:
		Index = 0;
		State = ANI_STATE::ANI_LOOP;
		break;
	case Charter::State_Walk:
		if (!isAttackReady)
			Index = State_Walk;
		else
			Index = 5;

		State = ANI_STATE::ANI_LOOP;
		break;
	case Charter::State_Wait:
		if (!isAttackReady)
			Index = State_Wait;
		else
			Index = 4;

		isAttack = false;
		State = ANI_STATE::ANI_LOOP;
		break;
	case Charter::State_InWeapon:
		Index = State_InWeapon;
		State = ANI_STATE::ANI_ONCE;
		break;
	case Charter::State_OutWeapon:
		Index = State_OutWeapon;
		State = ANI_STATE::ANI_ONCE;
		break;
	case Charter::State_Rapid1:
		Index = State_Rapid1;
		State = ANI_STATE::ANI_ONCE;
		isAttack = true;
		break;
	case Charter::State_Rapid2:
		Index = State_Rapid2;
		State = ANI_STATE::ANI_ONCE;
		isAttack = true;
		break;
	case Charter::State_Rapid2R:
		Index = State_Rapid2R;
		State = ANI_STATE::ANI_ONCE;
		break;
	case Charter::State_Rapid3:
		Index = State_Rapid3;
		State = ANI_STATE::ANI_ONCE;
		isAttack = true;
		break;
	}

	for (UINT i = 0; i < objects.size(); i++)
	{
		auto obj = dynamic_cast<SkinningModel*>(objects[i]);

		if (obj)
		{
			if (!obj->Play(Index, 0, State, 0.0f))
				return;
		}
	}

	PrevState = state;
}

void Charter::AnimationEvent(bool Result, int Index, UINT instanceID)
{
	if (Result)
	{
		if (Index == State_OutWeapon)
		{
			dynamic_cast<SkinningModel*>(objects[Part_BODY])->RemoveBoneTransform("Weapon_Back", 0);
			dynamic_cast<SkinningModel*>(objects[Part_BODY])->AddBoneTransform("R_Sword", 0, objects[Part_RWEAPON]->GetTransform(0));
			dynamic_cast<SkinningModel*>(objects[Part_BODY])->AddBoneTransform("L_Sword", 0, objects[Part_LWEAPON]->GetTransform(0));
			state = Charter::State_Wait;
			MoveSpeed = 9.5f;
			objects[Part_RWEAPON]->GetTransform(0)->SetPitchAngle(0.0f);
			objects[Part_RWEAPON]->GetTransform(0)->SetScaleFactor(1.0f);
			objects[Part_LWEAPON]->GetTransform(0)->SetPitchAngle(0.0f);
			objects[Part_LWEAPON]->GetTransform(0)->SetScaleFactor(1.0f);
			isAttackReady = true;
		}
		else if (Index == State_InWeapon)
		{
			dynamic_cast<SkinningModel*>(objects[Part_BODY])->RemoveBoneTransform("R_Sword" ,0);
			dynamic_cast<SkinningModel*>(objects[Part_BODY])->RemoveBoneTransform("L_Sword", 0);
			dynamic_cast<SkinningModel*>(objects[Part_BODY])->AddBoneTransform("Weapon_Back", 0, objects[Part_RWEAPON]->GetTransform(0));
			dynamic_cast<SkinningModel*>(objects[Part_BODY])->AddBoneTransform("Weapon_Back", 0, objects[Part_LWEAPON]->GetTransform(0));
			state = Charter::State_Wait;
			MoveSpeed = 4.5f;
			objects[Part_RWEAPON]->GetTransform(0)->SetPitchAngle(-45.0f * (D3DX_PI / 180.0f));
			objects[Part_RWEAPON]->GetTransform(0)->SetScaleFactor(0.6f);
			objects[Part_LWEAPON]->GetTransform(0)->SetPitchAngle(-60.0f * (D3DX_PI / 180.0f));
			objects[Part_LWEAPON]->GetTransform(0)->SetScaleFactor(0.6f);
			isAttackReady = false;
		}
		else if (Index == State_Combo1)
		{	
			Velocity = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			state = State_Combo1R;
		}
		else if (Index == State_Combo2)
		{
			Velocity = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			state = State_Combo2R;
		}
		else if (Index == State_Combo3)
		{
			Velocity = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			state = State_Combo3R;
		}
		else if (Index == State_Combo4)
		{
			Velocity = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			isAttack = false;
			AttackDelay = 0.0f;
			if (trail[0])
				trail[0]->SetActive(false);
			if (trail[1])
				trail[1]->SetActive(false);
			state = State_Wait;
		}
		else if (Index == State_Combo1R || Index == State_Combo2R || Index == State_Combo3R)
		{
			isAttack = false;
			AttackDelay = 0.0f;
			if(trail[0])
				trail[0]->SetActive(false);
			if(trail[1])
				trail[1]->SetActive(false);

			state = State_Wait;
		}
		else if (Index == State_Rapid1)
		{
			isAttack = false;

			trail[0]->SetActive(false);
			trail[1]->SetActive(false);
			state = State_Wait;
		}
		else if (Index == State_Rapid2)
		{
			state = State_Rapid2R;
		}
		else if (Index == State_Rapid2R)
		{
			isAttack = false;
			trail[0]->SetActive(false);
			trail[1]->SetActive(false);
			state = State_Wait;
		}
		else if (Index == State_Rapid3)
		{
			isAttack = false;
			trail[0]->SetActive(false);
			trail[1]->SetActive(false);
			state = State_Wait;
		}
	}
}
