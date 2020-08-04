#include "Framework.h"
#include "MainUI.h"

MainUI::MainUI()
{
	HpSprite = Engine->AddTexture("HPBar", L"Texture/UI/HpBar.png");
	MpSprite = Engine->AddTexture("MPBar", L"Texture/UI/MpBar.png");
	HpBackSprite = Engine->AddTexture("HpBack", L"Texture/UI/ProgressBar_I45.png");
	MpBackSprite = Engine->AddTexture("MpBack", L"Texture/UI/ProgressBar_I3A.png");
	CharterSprite = Engine->AddTexture("CharterWindow", L"Texture/UI/CharacterWindow_I177.png");
	EnemyHpShow = false;
	HpPos.x = 100;
	HpPos.y = Engine->GetClientHeight() - 100;
	MpPos.x = 100;
	MpPos.y = Engine->GetClientHeight() - 50;

	EnemyHpPos.x = 300.0f;
	EnemyHpPos.y = 50.0f;

	depth = new DepthStencilState();
	Sprite = make_unique<DirectX::SpriteBatch>(Engine->GetContext());
	BarBackrite = make_unique<DirectX::SpriteBatch>(Engine->GetContext());
	EnemySprite = make_unique<DirectX::SpriteBatch>(Engine->GetContext());
	EnemyBarBackSprite = make_unique<DirectX::SpriteBatch>(Engine->GetContext());
	EnemyHpDelay = 0.0f;
}

MainUI::~MainUI()
{
	delete depth;
}

void MainUI::Update(float DeltaTime)
{

}

void MainUI::Render()
{
	float hpWidth = 500 * (CurrentHP / MaxHP);
	float mpWidth = 500 * (CurrentMP / MaxMP);
	float EnemyHpWidth = 500 * (EnemyHP / EnemyMaxHP);

	SetRect(&rcHP, 0, 0, hpWidth, 13);
	SetRect(&rcHPBack, 0, 0, 480, 13);
	SetRect(&rcMP, 0, 0, mpWidth, 13);
	SetRect(&rcMPBack, 0, 0, 480, 32);
	SetRect(&rcCharterWindow, 0, 0, 100, 100);
	SetRect(&rcEnemyHP, 0, 0, EnemyHpWidth, 13);
	SetRect(&rcEnemyHPBack, 0, 0, 480, 13);

	if (EnemyHpShow)
	{
		EnemyBarBackSprite->Begin(DirectX::SpriteSortMode_Texture);

		EnemyBarBackSprite->Draw(HpBackSprite->GetSrv(), DirectX::XMFLOAT2(EnemyHpPos.x, EnemyHpPos.y), &rcEnemyHPBack, DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(2.5f, 1.0f), DirectX::SpriteEffects::SpriteEffects_None);

		EnemyBarBackSprite->End();

		EnemySprite->Begin(DirectX::SpriteSortMode_Texture);

		EnemySprite->Draw(HpSprite->GetSrv(), DirectX::XMFLOAT2(EnemyHpPos.x, EnemyHpPos.y), &rcEnemyHP, DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(1.5f, 1.0f), DirectX::SpriteEffects::SpriteEffects_None);
		EnemySprite->End();
		

	}

	BarBackrite->Begin(DirectX::SpriteSortMode_Texture);

	BarBackrite->Draw(MpBackSprite->GetSrv(), DirectX::XMFLOAT2(MpPos.x - 10.0f, MpPos.y - 10.0f), &rcMPBack, DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(2.5f, 1.0f), DirectX::SpriteEffects::SpriteEffects_None);
	BarBackrite->Draw(HpBackSprite->GetSrv(), DirectX::XMFLOAT2(HpPos.x, HpPos.y), &rcHPBack, DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(2.5f, 1.0f), DirectX::SpriteEffects::SpriteEffects_None);

	BarBackrite->End();

	Sprite->Begin(DirectX::SpriteSortMode_Texture);

	Sprite->Draw(HpSprite->GetSrv(), DirectX::XMFLOAT2(HpPos.x, HpPos.y), &rcHP, DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(1.5f, 1.0f), DirectX::SpriteEffects::SpriteEffects_None);
	Sprite->Draw(MpSprite->GetSrv(), DirectX::XMFLOAT2(MpPos.x, MpPos.y), &rcMP, DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(1.5f, 1.0f), DirectX::SpriteEffects::SpriteEffects_None);
	Sprite->Draw(CharterSprite->GetSrv(), DirectX::XMFLOAT2(-35.0, Engine->GetClientHeight() - 170.0f), &rcCharterWindow, DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), 1.5f, DirectX::SpriteEffects::SpriteEffects_None);

	Sprite->End();



	depth->Set();
}

void MainUI::SetEnemyHpShow(bool val, float DeltaTime)
{
	if (!val)
	{
		EnemyHpDelay += DeltaTime;

		if (EnemyHpDelay >= 4.0f)
		{
			EnemyHpDelay = 0.0f;
			EnemyHpShow = false;
		}
	}
	else
	{
		EnemyHpDelay = 0.0f;
		EnemyHpShow = true;
	}
}

void MainUI::SetPlayerInfo(float CurrentHP, float MaxHP, float CurrentMP, float MaxMP)
{
	this->CurrentHP = CurrentHP;
	this->MaxHP = MaxHP;
	this->CurrentMP = CurrentMP;
	this->MaxMP = MaxMP;
}

void MainUI::SetEnemyInfo(float EnemyHP, float EnemyMaxHP)
{
	this->EnemyHP = EnemyHP;
	this->EnemyMaxHP = EnemyMaxHP;
}
