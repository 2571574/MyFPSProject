#pragma once
#include "DxLib.h"
class Player;

class HUD
{
private:
	Player* pplayer;
	float cancelReloadTimer;

	float hitMarkTimer = 0.0f;
	bool lastHitWasHS = false;

public:
	HUD(Player* player);
	~HUD() = default;
	void Update();
	void Draw();

	void OnHitTarget(bool isHeadShot);
};

