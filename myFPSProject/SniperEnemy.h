#pragma once
#include "Enemy.h"
#include "Weapon.h"
class SniperEnemy :
    public Enemy
{
private:
    std::unique_ptr<Weapon> sniper;
    float attackDist;   //攻撃距離
	float escapeDist;   //逃走距離
    float targetingTimer;   //狙っている間のタイマー
    const float TARGET_TIME = 4.5f; //狙って撃つまでの時間

    int chargeSoundHandle;  //チャージ音ハンドル

public:
    SniperEnemy(VECTOR pos, Player* target);

    void Update()override;
    void Draw()override;
    void Action()override;
};

