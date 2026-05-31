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
	bool damageText;		//ダメージテキスト表示フラグ
	std::vector<DamageText> damageTexts;	//ダメージテキストの配列
	float accumulateTimer;	//ダメージの累積許容時間のタイマー

	int fontDamage = -1;	//ダメージテキストのフォントハンドル
public:
	Dummy(VECTOR pos, Player* _target,bool damageText);

	void Update() override;
	void Draw() override;
	void Action() override;
	void OnHit(int damage,WeaponID id = WeaponID::UNKNOWN) override;

};
