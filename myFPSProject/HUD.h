#pragma once
#include "DxLib.h"
class Player;

/// <summary>
/// HUD全般を管理するクラス
/// </summary>
class HUD
{
private:
	Player* pplayer;	//プレイヤーのポインタ

	float hitMarkTimer = 0.0f;	//ヒットマークの表示タイマー
	bool lastHitWasHS = false;	//最後の命中がヘッドショットだったかどうか
	bool lastHitWasKill = false;//最後の命中がキルだったかどうか

	bool isGameStarted = false;	//ゲーム開始フラグ　
	float scoreFadeAlpha = 0.0f;//スコアのフェードインアルファ

	float damageFlashTimer = 0.0f;//ダメージ演出の表示タイマー



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

	/// <summary>
	/// ターゲットに命中したことを感知する
	/// </summary>
	/// <param name="isHeadShot">ヘッドショットかどうか</param>
	/// <param name="isKill">キル判定になったかどうか</param>
	void OnHitTarget(bool isHeadShot,bool isKill);

	/// <summary>
	/// プレイヤーがダメージを受けたことを感知する
	/// </summary>
	void OnPlayerTakeDamage();
};

