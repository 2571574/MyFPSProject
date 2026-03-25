#include "Dummy.h"
#include "Time.h"
#include "ResourceManager.h"
constexpr float TEXT_LIFETIME = 1.0f;
constexpr float TEXT_HEIGHT_OFFSET = 0.5f;
constexpr float ACCUMULATE_TIME_MAX = 0.2f;
/*コンストラクタ*/
Dummy::Dummy(VECTOR pos, Player* _target,bool _damageText) :Enemy(pos, CHARA_STATUS::DUMMY, _target, ENEMYTYPE::DUMMY), accumulateTimer(0.0f), damageText(_damageText) {
	nowSpawned = false;
	fontDamage = ResourceManager::GetIns().GetFont("Resource/Font/RobotoMono_24.dft");
};

/*更新*/
void Dummy::Update() {
	float dt = Time::GetIns().GetDelta();
	if (onHitFlashTimer > 0.0f) {onHitFlashTimer -= dt;}
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
	VECTOR cPos = GetPos();
	float bodyRad = status.width / 2.0f;
	float headRad = bodyRad * 0.5f;
	if (headRad < 0.15f)headRad = 0.15f;
	VECTOR bottom = VAdd(position, VGet(0.0f, bodyRad, 0.0f));
	float neck = status.eyeHeight - headRad;
	VECTOR bodyTop = VAdd(cPos, VGet(0.0f, neck - bodyRad, 0.0f));
	VECTOR headPos = VAdd(cPos, VGet(0.0f, currentEyeHeight, 0.0f));
	unsigned int color =GetColor(255, 50, 50);
	if (onHitFlashTimer > 0.0f)color = GetColor(255, 255, 255);

	int fillFlag = nowSpawned ? FALSE : TRUE;

	if (!nowSpawned) {
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 100);
		VECTOR shadowPos1 = VAdd(position, VGet(0.0f, 0.02f, 0.0f));
		VECTOR shadowPos2 = VAdd(position, VGet(0.0f, 0.01f, 0.0f));
		DrawCone3D(shadowPos1, shadowPos2, status.width / 1.5f, 16, GetColor(0, 0, 0), GetColor(0, 0, 0), TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
	//カプセルを描画
	DrawCapsule3D(bottom, bodyTop, bodyRad, CIRCLE_DIVNUM, color, color, fillFlag);
	DrawSphere3D(headPos, headRad, CIRCLE_DIVNUM, color, color, fillFlag);

	for (const auto& text : damageTexts) {
		VECTOR screenPos = ConvWorldPosToScreenPos(text.pos);

		if (screenPos.z >= 0.0f && screenPos.z <= 1.0f) {
			float alphaRate = text.lifeTime / 1.0f; 
			if (alphaRate < 0.0f) alphaRate = 0.0f;
			int alpha = static_cast<int>(255 * alphaRate);

			::SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
			// フォントハンドルを指定し、色は白に変更
			::DrawFormatStringToHandle(static_cast<int>(screenPos.x), static_cast<int>(screenPos.y), GetColor(255, 255, 255), fontDamage, "%d", text.damage);
			::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		}
	}
}

void Dummy::Action() {

}

void Dummy::OnHit(int damage, WeaponID id) {
	if (!alive) return;
	TakeDamage(damage, id);
	onHitFlashTimer = 0.1f;
	if (!damageText)return;
	if (accumulateTimer > 0.0f && !damageTexts.empty()) {
		DamageText& lastText = damageTexts.back();

		lastText.damage += damage;
		lastText.lifeTime = TEXT_LIFETIME;
		lastText.pos = VAdd(position, VGet(0, currentHeight + TEXT_HEIGHT_OFFSET, 0));
	}
	else {
		VECTOR textPos = VAdd(position, VGet(0, currentHeight + TEXT_HEIGHT_OFFSET, 0));
		damageTexts.push_back({ damage,textPos,TEXT_LIFETIME });
	}

	accumulateTimer = ACCUMULATE_TIME_MAX;
}