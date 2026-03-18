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

	bool isGameStarted = false;
	float scoreFadeAlpha = 0.0f;

public:
	HUD(Player* player);
	~HUD() = default;
	void Update();
	void Draw();

	void OnHitTarget(bool isHeadShot);
};

