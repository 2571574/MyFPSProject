#pragma once
#include "Enemy.h"
#include "Weapon.h"
class SniperEnemy :
    public Enemy
{
private:
    std::unique_ptr<Weapon> sniper;
    float attackDist;
    float escapeDist;
    float targetingTimer;
    const float TARGET_TIME = 3.0f;

public:
    SniperEnemy(VECTOR pos, Player* target);
    void Update()override;
    void Draw()override;
    void Action()override;
};

