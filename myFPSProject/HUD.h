#pragma once
#include "DxLib.h"
class Player;

/// <summary>
/// HUD全般を管理するクラス
/// </summary>
class HUD
{
private:
	Player* pplayer;

	float hitMarkTimer = 0.0f;
	bool lastHitWasHS = false;
	bool lastHitWasKill = false;

	bool isGameStarted = false;
	float scoreFadeAlpha = 0.0f;

	float damageFlashTimer = 0.0f;
	const float DAMAGE_FLASH_DURATION = 1.0f;


	int fontJpLarge = -1;   // スコア、スタートタイマー用 (メイリオ)
	int fontJpMedium = -1;  // 武器名、PickUp表記用 (メイリオ)
	int fontEnLarge = -1;   // AMMOのメイン数字用 (Century Gothic)
	int fontEnSmall = -1;   // 予備弾数、RELOADING用 (Century Gothic)

	const int PICKUP_ICON_SIZE = 100; // ピックアップUIのアイコンサイズ
public:
	HUD(Player* player);
	~HUD() = default;
	void Update();
	void Draw();

	void OnHitTarget(bool isHeadShot,bool isKill);
	void OnPlayerTakeDamage();
};

