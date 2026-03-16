#include "Dummy.h"
#include "Time.h"
/*コンストラクタ*/
Dummy::Dummy(VECTOR pos, Player* _target,bool _damageText) :Enemy(pos, CHARA_STATUS::DUMMY, _target, ENEMYTYPE::DUMMY), accumulateTimer(0.0f), damageText(_damageText) {};

/*更新*/
void Dummy::Update() {
	float dt = Time::GetIns().GetDelta();
	if (accumulateTimer > 0.0f) { accumulateTimer -= dt; }
	for (auto n = damageTexts.begin(); n != damageTexts.end();) {
		n->lifeTime -= dt;
		n->pos.y += dt;

		if (n->lifeTime <= 0.0f) {
			n = damageTexts.erase(n);
		}
		else {
			++n;
		}
	}
	
}

/*描画*/
void Dummy::Draw() {
	float bodyRad = status.width / 2.0f;
	VECTOR bottom = VAdd(position, VGet(0.0f, bodyRad, 0.0f));
	VECTOR top = VAdd(position, VGet(0, currentHeight - bodyRad, 0));

	unsigned int color = alive ? GetColor(255, 50, 50) : GetColor(50, 50, 50);	//色
	//カプセルを描画
	DrawCapsule3D(bottom, top, bodyRad, CIRCLE_DIVNUM, color,color, TRUE);

	for (const auto& text : damageTexts) {
		VECTOR screenPos = ConvWorldPosToScreenPos(text.pos);

		if (screenPos.z >= 0.0f && screenPos.z <= 1.0f) {
			DrawFormatString(screenPos.x, screenPos.y, GetColor(0, 0, 0), "%d", text.damage);
		}
	}
}

void Dummy::Action() {

}

void Dummy::OnHit(int damage, WeaponID id) {
	if (!alive) return;
	TakeDamage(damage, id);
	if (!damageText)return;
	if (accumulateTimer > 0.0f && !damageTexts.empty()) {
		DamageText& lastText = damageTexts.back();

		lastText.damage += damage;
		lastText.lifeTime = 1.0f;
		lastText.pos = VAdd(position, VGet(0, currentHeight + 0.5f, 0));
	}
	else {
		VECTOR textPos = VAdd(position, VGet(0, currentHeight + 0.5f, 0));
		damageTexts.push_back({ damage,textPos,1.0f });
	}

	accumulateTimer = 0.2f;
}