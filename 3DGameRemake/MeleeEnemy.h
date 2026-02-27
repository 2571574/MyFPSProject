#pragma once
#include "Enemy.h"
class MeleeEnemy :
    public Enemy
{
private:
    float attackTimer;
    float range;
    float moveSpeed;

public:
    MeleeEnemy(VECTOR pos, Player* target);

    void Update() override;
    void Draw() override;
    void Action() override;
};

