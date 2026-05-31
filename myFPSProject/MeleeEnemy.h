#pragma once
#include "Enemy.h"
#include "Weapon.h"
#include <memory>

//攻撃状態
enum class MeleeState {
    IDLE,
    ATTACK_WIND,
    ATTACK_RECOVERY
};

/// <summary>
/// 近接型人型敵クラス
/// </summary>
class MeleeEnemy :
    public Enemy
{
private:
	MeleeState state = MeleeState::IDLE;    //現在の攻撃状態
    float stateTimer = 0.0f;        //攻撃状態管理用のタイマー
	float attackTimer;  //攻撃のクールタイム
    std::unique_ptr<Weapon> melee;  //武器

public:
    MeleeEnemy(VECTOR pos, Player* target);

    void Update() override;
    void Draw() override;
    void Action() override;
};

