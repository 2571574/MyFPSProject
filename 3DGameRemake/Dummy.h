#pragma once
#include "Enemy.h"
class Player;
//ダメージテキスト
struct DamageText {
	int damage;
	VECTOR pos;
	float lifeTime;
};
/// <summary>
/// 的となる敵のクラス
/// </summary>
class Dummy:public Enemy
{
private:
	bool damageText;
	std::vector<DamageText> damageTexts;	//ダメージテキストの配列
	float accumulateTimer;					//ダメージの累積を許す時間のタイマー
public:
	Dummy(VECTOR pos, Player* _target,bool damageText);

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update() override;

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw() override;

	/// <summary>
	/// 攻撃処理　攻撃しないので空
	/// </summary>
	void Action() override;

	/// <summary>
	/// 被弾処理
	/// </summary>
	/// <param name="damage">喰らったダメージ量</param>
	void OnHit(int damage,WeaponID id = WeaponID::UNKNOWN) override;

};
