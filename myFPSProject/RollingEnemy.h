#pragma once
#include "Enemy.h"

/// <summary>
/// 転がる敵を管理するクラス
/// </summary>
class RollingEnemy :
    public Enemy
{
private:
    bool isExploding;   //爆発中のフラグ
    float explodeTimer; //爆発中管理用タイマー
	float triggerDist;  //爆発開始距離
    GunStatus explodeSpec;  //爆発の仕様

    float alerttimer;  //警告タイマー
    float alertDuration;  //爆発までの時間
public:

    RollingEnemy(VECTOR pos, Player* target);
    void Update()override;
    void Draw()override;
    void Action()override;

    /// <summary>
    /// 壁や床の判定　滑るため独自化
    /// </summary>
    void UpdatePhysics();

	/// <summary>
	/// 経路探索　滑っても違和感ないよう独自化
	/// </summary>
	/// <param name="target">ターゲットのプレイヤー</param>
	/// <param name="dt">時間</param>
	/// <returns>次に向かう方向</returns>
	VECTOR UpdateNavigation(const Character* target, float dt);
};

