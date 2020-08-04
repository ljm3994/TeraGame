#pragma once
#include "FrameWork\States\DepthStencilState.h"
class MainUI
{
	RECT rcHP;
	RECT rcHPBack;
	RECT rcMP;
	RECT rcMPBack;
	RECT rcCharterWindow;
	RECT rcEnemyHPBack;
	RECT rcEnemyHP;

	unique_ptr<DirectX::SpriteBatch> Sprite;
	unique_ptr<DirectX::SpriteBatch> EnemySprite;
	unique_ptr<DirectX::SpriteBatch> BarBackrite;
	unique_ptr<DirectX::SpriteBatch> EnemyBarBackSprite;

	D3DXVECTOR3 HpPos;
	D3DXVECTOR3 MpPos;
	D3DXVECTOR3 EnemyHpPos;

	bool EnemyHpShow;
	Texture* HpSprite;
	Texture* MpSprite;
	Texture* HpBackSprite;
	Texture* MpBackSprite;
	Texture* CharterSprite;

	DepthStencilState* depth;

	float CurrentHP;
	float MaxHP;
	float CurrentMP;
	float MaxMP;

	float EnemyHP;
	float EnemyMaxHP;

	float EnemyHpDelay;
public:
	MainUI();
	~MainUI();

	void Update(float DeltaTime);
	void Render();

	void SetEnemyHpShow(bool val, float DeltaTime);
	void SetPlayerInfo(float CurrentHP, float MaxHP, float CurrentMP, float MaxMP);
	void SetEnemyInfo(float EnemyHP, float EnemyMaxHP);
};