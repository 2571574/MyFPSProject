#pragma once
#include "DxLib.h"
class Player;

class HUD
{
private:
	Player* pplayer;
	float cancelReloadTimer;
public:
	HUD(Player* player);
	~HUD() = default;
	void Update();
	void Draw();
};

