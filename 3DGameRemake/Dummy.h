#pragma once
#include "Enemy.h"
class Player;
struct DamageText {
	int damage;
	VECTOR pos;
	float lifeTime;
};
/*敵(射撃の的)*/
class Dummy:public Enemy
{
private:
	std::vector<DamageText> damageTexts;
	float accumulateTimer;
public:
	Dummy(VECTOR pos,Player*_target);		//コンストラクタ

	void Update() override;	//更新
	void Draw() override;	//描画
	void Action() override;

	void OnHit(int damage) override;

};
