#pragma once
#include "Enemy.h"
#include "Weapon.h"
#include <memory>

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
    MeleeState state = MeleeState::IDLE;
    float stateTimer = 0.0f;
	float attackTimer;  //攻撃のクールタイム
    std::unique_ptr<Weapon> melee;
public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    /// <param name="pos">初期座標</param>
    /// <param name="target">ターゲットのプレイヤー</param>
    MeleeEnemy(VECTOR pos, Player* target);

    /// <summary>
    /// 更新処理
    /// </summary>
    void Update() override;

    /// <summary>
    /// 描画処理
    /// </summary>
    void Draw() override;

    /// <summary>
    /// 攻撃処理
    /// </summary>
    void Action() override;
};

