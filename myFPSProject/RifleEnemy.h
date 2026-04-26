#pragma once
#include "Enemy.h"
#include "Weapon.h"
class RifleEnemy :
    public Enemy
{
private:
	float range;	//攻撃距離
	float stopDist;		//止まる距離

	std::unique_ptr<Weapon> rifle;  //武器

public:
    RifleEnemy(VECTOR pos, Player* target);
	void Update() override;
	void Draw() override;
	void Action() override;
};

