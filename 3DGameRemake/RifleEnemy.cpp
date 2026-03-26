#include "RifleEnemy.h"
#include "Player.h"
#include "Time.h"
#include "EnemyManager.h"
#include "Status.h"
#include "Param/Global.h"
#include "Param/Chara.h"
#include "Param/System.h"


RifleEnemy::RifleEnemy(VECTOR pos, Player* target)
	: Enemy(pos, CHARA_STATUS::RIFLE_ENEMY, target, ENEMYTYPE::RIFLE) {

	rifle = std::make_unique<Weapon>(ENEMY_GUN::RIFLE);
	range = rifle->GetSpec().range;
	stopDist = range * Chara::Rifle::STOP_DISTANCE_RATIO;
}

void RifleEnemy::Update() {
	float dt = Time::GetIns().GetDelta();
	if (onHitFlashTimer > 0.0f) onHitFlashTimer -= dt;
	if (nowSpawned) {
		spawnedTimer -= dt;
		if (spawnedTimer <= 0.0f) {
			nowSpawned = false;
		}
		return;
	}

	if (hp <= 0) {
		alive = false;
		return;
	}

	if (rifle) {
		rifle->Update(*this);
	}
	if (target == nullptr) return;

	VECTOR moveTarget = UpdateNavigation(target, dt);

	VECTOR dir = VNorm(VSub(moveTarget, position));
	dir.y = 0.0f;

	if (VSize(VSub(target->GetPos(), position)) > stopDist) {
		ApplyMovement(dir, stageHandle);
	}
	else {
		ApplyMovement(VGet(0.0f, 0.0f, 0.0f), stageHandle);
	}

	if (CheckLineSight(target, target->GetCurrentEyeHeight())) {
		Action();
	}

	UpdateFootstep();
}

void RifleEnemy::Action() {
	VECTOR eyePos = VAdd(position, VGet(0.0f, status.eyeHeight, 0.0f));
	VECTOR targetEyePos = VAdd(target->GetPos(), VGet(0.0f, target->GetCurrentEyeHeight(), 0.0f));
	VECTOR fireDir = VNorm(VSub(targetEyePos, eyePos));
	if (rifle) {
		if (rifle->GetAmmo() <= 0) {
			rifle->Reload(*this);
		}
		if (!rifle->Reloading()) {
			rifle->Fire(*this, fireDir);
		}
	}
}

void RifleEnemy::Draw() {
	VECTOR cPos = GetPos();
	float bodyRad = status.width / System::Collision::BODY_RADIUS_DIVISOR;
	float headRad = bodyRad / System::Collision::HEAD_RADIUS_DIVISOR;
	if (headRad < System::Collision::MIN_HEAD_RAD) headRad = System::Collision::MIN_HEAD_RAD;
	VECTOR bottom = VAdd(position, VGet(0.0f, bodyRad, 0.0f));
	float neck = status.eyeHeight - headRad;
	VECTOR bodyTop = VAdd(cPos, VGet(0.0f, neck - bodyRad, 0.0f));
	VECTOR headPos = VAdd(cPos, VGet(0.0f, currentEyeHeight, 0.0f));

	VECTOR leanMax = VGet(velocity.x, 0.0f, velocity.z);
	leanMax = VScale(leanMax, Chara::EnemyCommon::LEAN_FACTOR);

	float headHeight = currentEyeHeight - bodyRad;
	float bodyTopHeight = (neck - bodyRad) - bodyRad;

	VECTOR bodyTopLean = VScale(leanMax, bodyTopHeight / headHeight);

	bodyTop = VAdd(bodyTop, bodyTopLean);
	headPos = VAdd(headPos, leanMax);
	int color = GetColor(Chara::Rifle::COLOR_NORMAL.r, Chara::Rifle::COLOR_NORMAL.g, Chara::Rifle::COLOR_NORMAL.b);
	if (onHitFlashTimer > 0.0f) color = GetColor(Global::Palette::WHITE.r, Global::Palette::WHITE.g, Global::Palette::WHITE.b);

	int fillFlag = nowSpawned ? FALSE : TRUE;

	if (!nowSpawned) {
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, Chara::Base::SHADOW_ALPHA);
		VECTOR shadowPos1 = VAdd(position, VGet(0.0f, Chara::Base::SHADOW_OFFSET_Y_HIGH, 0.0f));
		VECTOR shadowPos2 = VAdd(position, VGet(0.0f, Chara::Base::SHADOW_OFFSET_Y_LOW, 0.0f));
		DrawCone3D(shadowPos1, shadowPos2, status.width / Chara::Base::SHADOW_WIDTH_DIVISOR, Chara::Base::SHADOW_CONE_SEGMENTS, GetColor(Global::Palette::BLACK.r, Global::Palette::BLACK.g, Global::Palette::BLACK.b), GetColor(Global::Palette::BLACK.r, Global::Palette::BLACK.g, Global::Palette::BLACK.b), TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}

	DrawCapsule3D(bottom, bodyTop, bodyRad, System::Window::CIRCLE_DIVNUM, color, color, fillFlag);
	DrawSphere3D(headPos, headRad, System::Window::CIRCLE_DIVNUM, color, color, fillFlag);

	if (rifle) {
		VECTOR forward = VGet(0.0f, 0.0f, 1.0f);
		if (target) {
			forward = VNorm(VSub(target->GetPos(), position));
			forward.y = 0.0f;
			if (VSize(forward) < System::Collision::MIN_DIST_SQUARED) forward = VGet(0.0f, 0.0f, 1.0f);
		}
		VECTOR right = VNorm(VCross(VGet(0.0f, 1.0f, 0.0f), forward));
		VECTOR up = VNorm(VCross(forward, right));

		VECTOR drawPos = VAdd(position, VGet(0.0f, status.height * Chara::Base::HALF_RATIO, 0.0f));
		float weaponHeight = (status.height * Chara::Base::HALF_RATIO) - bodyRad;
		drawPos = VAdd(drawPos, VScale(leanMax, weaponHeight / headHeight));
		rifle->Draw(drawPos, forward, right, up, false, false);
	}
}