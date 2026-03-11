#pragma once
#include "Enemy.h"
#include "Weapon.h"
class RifleEnemy :
    public Enemy
{
private:
	float range;
	float stopDist;		//止まる距離
	float moveSpeed;

	std::unique_ptr<Weapon> rifle;

public:
    RifleEnemy(VECTOR pos, Player* target);
	void Update() override;
	void Draw() override;
	void Action() override;
};

