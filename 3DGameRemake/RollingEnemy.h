#pragma once
#include "Enemy.h"
class RollingEnemy :
    public Enemy
{
private:
    bool isExploding;
    float explodeTimer;
    float triggerDist;
    GunStatus explodeSpec;
public:
    RollingEnemy(VECTOR pos, Player* target);
    void Update()override;
    void Draw()override;
    void Action()override;
	void OnHit(int damage)override;
    void UpdatePhysics(float dt);
	VECTOR UpdateNavigation(const Character* target, float dt);
};

