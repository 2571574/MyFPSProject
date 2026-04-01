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



	int fontJpLarge = -1;   // スコア、スタートタイマー用
	int fontJpMedium = -1;  // 武器名、PickUp表記用
	int fontEnLarge = -1;   // AMMOのメイン数字用
	int fontEnSmall = -1;   // 予備弾数、RELOADING用

	const int PICKUP_ICON_SIZE = 100; // ピックアップUIのアイコンサイズ
public:
	HUD(Player* player);
	~HUD() = default;
	void Update();
	void Draw();

	void OnHitTarget(bool isHeadShot,bool isKill);
	void OnPlayerTakeDamage();
};

