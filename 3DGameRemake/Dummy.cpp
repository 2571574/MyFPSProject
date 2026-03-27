#include "Dummy.h"
#include "Time.h"
#include "ResourceManager.h"
#include "Param/Global.h"
#include "Param/Chara.h"
#include "Param/System.h"
#include "Param/Visual.h"

/*コンストラクタ*/
Dummy::Dummy(VECTOR pos, Player* _target, bool _damageText) :Enemy(pos, CHARA_STATUS::DUMMY, _target, ENEMYTYPE::DUMMY), accumulateTimer(0.0f), damageText(_damageText) {
	nowSpawned = false;
	fontDamage = ResourceManager::GetIns().GetFont("Resource/Font/RobotoMono_24.dft");
};

/*更新*/
void Dummy::Update() {
	float dt = Time::GetIns().GetDelta();
	if (onHitFlashTimer > 0.0f) { onHitFlashTimer -= dt; }
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
	float headRad = bodyRad / 2.0f;
	if (headRad < System::Collision::MIN_HEAD_RAD) headRad = System::Collision::MIN_HEAD_RAD;
	VECTOR bottom = VAdd(position, VGet(0.0f, bodyRad, 0.0f));
	float neck = status.eyeHeight - headRad;
	VECTOR bodyTop = VAdd(cPos, VGet(0.0f, neck - bodyRad, 0.0f));
	VECTOR headPos = VAdd(cPos, VGet(0.0f, currentEyeHeight, 0.0f));

	int color = GetColor(Chara::Dummy::COLOR_NORMAL.r, Chara::Dummy::COLOR_NORMAL.g, Chara::Dummy::COLOR_NORMAL.b);
	if (onHitFlashTimer > 0.0f) color = GetColor(Global::Palette::WHITE.r, Global::Palette::WHITE.g, Global::Palette::WHITE.b);

	int fillFlag = nowSpawned ? FALSE : TRUE;

	if (!nowSpawned) {
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, Chara::Base::SHADOW_ALPHA);
		VECTOR shadowPos1 = VAdd(position, VGet(0.0f, Chara::Base::SHADOW_OFFSET_Y_HIGH, 0.0f));
		VECTOR shadowPos2 = VAdd(position, VGet(0.0f, Chara::Base::SHADOW_OFFSET_Y_LOW, 0.0f));
		DrawCone3D(shadowPos1, shadowPos2, status.width / Chara::Base::SHADOW_WIDTH_DIVISOR, Chara::Base::SHADOW_CONE_SEGMENTS, GetColor(Global::Palette::BLACK.r, Global::Palette::BLACK.g, Global::Palette::BLACK.b), GetColor(Global::Palette::BLACK.r, Global::Palette::BLACK.g, Global::Palette::BLACK.b), TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
	//カプセルを描画
	DrawCapsule3D(bottom, bodyTop, bodyRad, System::Window::CIRCLE_DIVNUM, color, color, fillFlag);
	DrawSphere3D(headPos, headRad, System::Window::CIRCLE_DIVNUM, color, color, fillFlag);

	for (const auto& text : damageTexts) {
		VECTOR screenPos = ConvWorldPosToScreenPos(text.pos);

		if (screenPos.z >= 0.0f && screenPos.z <= 1.0f) {
			float alphaRate = text.lifeTime / Chara::Dummy::TEXT_LIFETIME;
			if (alphaRate < 0.0f) alphaRate = 0.0f;
			int alpha = static_cast<int>(255 * alphaRate);

			::SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
			::DrawFormatStringToHandle(static_cast<int>(screenPos.x), static_cast<int>(screenPos.y), GetColor(Global::Palette::WHITE.r, Global::Palette::WHITE.g, Global::Palette::WHITE.b), fontDamage, "%d", text.damage);
			::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		}
	}
}

void Dummy::Action() {}

void Dummy::OnHit(int damage, WeaponID id) {
	if (!alive) return;
	TakeDamage(damage, id);
	onHitFlashTimer = Chara::Dummy::HIT_FLASH_DURATION;
	if (!damageText) return;
	if (accumulateTimer > 0.0f && !damageTexts.empty()) {
		DamageText& lastText = damageTexts.back();

		lastText.damage += damage;
		lastText.lifeTime = Chara::Dummy::TEXT_LIFETIME;
		lastText.pos = VAdd(position, VGet(0.0f, currentHeight + Chara::Dummy::TEXT_HEIGHT_OFFSET, 0.0f));
	}
	else {
		VECTOR textPos = VAdd(position, VGet(0.0f, currentHeight + Chara::Dummy::TEXT_HEIGHT_OFFSET, 0.0f));
		damageTexts.push_back({ damage, textPos, Chara::Dummy::TEXT_LIFETIME });
	}

	accumulateTimer = Chara::Dummy::ACCUMULATE_TIME_MAX;
}