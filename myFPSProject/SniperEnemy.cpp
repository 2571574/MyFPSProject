#include "SniperEnemy.h"
#include "Player.h"
#include "Status.h"
#include "Time.h"
#include "SoundManager.h"
#include "Param/Global.h"
#include "Param/Chara.h"
#include "Param/System.h"
#include "Param/Visual.h"

SniperEnemy::SniperEnemy(VECTOR pos, Player* target)
	: Enemy(pos, CHARA_STATUS::SNIPER_ENEMY, target, ENEMYTYPE::SNIPER)
	, targetingTimer(0.0f)
	, chargeSoundHandle(-1) {
	sniper = std::make_unique<Weapon>(ENEMY_GUN::SNIPER);
	attackDist = sniper->GetSpec().range * Chara::Sniper::ATTACK_DISTANCE_RATIO;
	escapeDist = sniper->GetSpec().range * Chara::Sniper::ESCAPE_DISTANCE_RATIO;
}

void SniperEnemy::Update() {
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
		if (chargeSoundHandle != -1) {
			SoundManager::GetIns().StopSE(chargeSoundHandle);
			chargeSoundHandle = -1;
		}
		alive = false;
		return;
	}

	if (sniper)sniper->Update(*this);
	if (target == nullptr)return;

	//移動
	VECTOR moveTarget = UpdateNavigation(target, dt);
	float distToPlayer = VSize(VSub(target->GetPos(), position));
	bool hasLos = CheckLineSight(target, target->GetCurrentHeight() * 0.5f);
	VECTOR moveDir = VGet(0.0f, 0.0f, 0.0f);


	//行動パターン

	//プレイヤーが近すぎる場合は距離を取る
	if (distToPlayer < escapeDist) {
		VECTOR escapeDir = VNorm(VSub(position, target->GetPos()));

		if (CheckPathSafety(VAdd(position, VScale(escapeDir, Chara::Sniper::ESCAPE_CHECK_DIST)))) {
			moveDir = escapeDir;
		}
		else {
			moveDir = VGet(0.0f, 0.0f, 0.0f);
		}
	}
	//攻撃距離より遠い、または視線が通っていない場合は近づく
	else if (distToPlayer > attackDist || !hasLos) {
		moveDir = VNorm(VSub(moveTarget, position));
	}

	//狙っている間はプレイヤーの方を向くが、プレイヤーの方に移動しない
	if (targetingTimer > 0.0f) {
		VECTOR toPlayer = VSub(target->GetPos(), position);
		toPlayer.y = 0.0f;

		if (VDot(moveDir, toPlayer) > 0.0f) {
			moveDir = VGet(0.0f, 0.0f, 0.0f);
		}
	}
	moveDir.y = 0.0f;
	ApplyMovement(moveDir, stageHandle);

	if (sniper->GetAmmo() <= 0 && !sniper->Reloading()) {
		sniper->Reload(*this);
	}

	//弾がある、プレイヤーが視界内、射程内なら攻撃
	if (sniper->CanFire() && CheckLineSight(target, target->GetCurrentHeight() * 0.5f) && distToPlayer <= sniper->GetSpec().range) {

		if (targetingTimer == 0.0f) {
			chargeSoundHandle = SoundManager::GetIns().Play3DSE("Resource/Sound/SRCharge.wav", position, Chara::Sniper::CHARGE_SOUND_RADIUS);
		}

		targetingTimer += dt;
		if (target) {
			float progress = targetingTimer / Chara::Sniper::TARGET_TIME;
			if (progress > 1.0f) progress = 1.0f;
			target->AddTargeted(position, progress);
		}
		if (targetingTimer >= Chara::Sniper::TARGET_TIME) {
			Action();
			targetingTimer = 0.0f;
		}
	}
	else {
		if (targetingTimer > 0.0f) {
			SoundManager::GetIns().StopSE(chargeSoundHandle);
		}
		targetingTimer = 0.0f;
	}

	UpdateFootstep();
}

void SniperEnemy::Action() {
	if (!sniper || sniper->Reloading())return;

	SoundManager::GetIns().StopSE("Resource/Sound/charge.wav");
	VECTOR s = VAdd(position, VGet(0.0f, status.eyeHeight, 0.0f));
	VECTOR e = VAdd(target->GetPos(), VGet(0.0f, target->GetCurrentHeight() * 0.5f, 0.0f));
	VECTOR fireDir = VNorm(VSub(e, s));

	sniper->Fire(*this, fireDir);
}

void SniperEnemy::Draw() {
	VECTOR cPos = GetPos();
	float bodyRad = status.width / 2.0f;
	float headRad = bodyRad / 2.0f;
	if (headRad < System::Collision::MIN_HEAD_RAD) headRad = System::Collision::MIN_HEAD_RAD;
	VECTOR bottom = VAdd(position, VGet(0.0f, bodyRad, 0.0f));
	float neck = status.eyeHeight - headRad;
	VECTOR bodyTop = VAdd(cPos, VGet(0.0f, neck - bodyRad, 0.0f));
	VECTOR headPos = VAdd(cPos, VGet(0.0f, currentEyeHeight, 0.0f));

	//速度に応じて体を傾ける
	VECTOR leanMax = VGet(velocity.x, 0.0f, velocity.z);
	leanMax = VScale(leanMax, Chara::EnemyCommon::LEAN_FACTOR);

	float headHeight = currentEyeHeight - bodyRad;
	float bodyTopHeight = (neck - bodyRad) - bodyRad;

	VECTOR bodyTopLean = VScale(leanMax, bodyTopHeight / headHeight);

	bodyTop = VAdd(bodyTop, bodyTopLean);
	headPos = VAdd(headPos, leanMax);

	int color = GetColor(Visual::Effect::COLOR_EFFECT_SNIPER.r, Visual::Effect::COLOR_EFFECT_SNIPER.g, Visual::Effect::COLOR_EFFECT_SNIPER.b);
	if (onHitFlashTimer > 0.0f) color = GetColor(Global::Palette::WHITE.r, Global::Palette::WHITE.g, Global::Palette::WHITE.b);

	int fillFlag = nowSpawned ? FALSE : TRUE;

	if (!nowSpawned) {
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, Chara::Base::SHADOW_ALPHA);
		VECTOR shadowPos1 = VAdd(position, VGet(0.0f, Chara::Base::SHADOW_OFFSET_Y_HIGH, 0.0f));
		VECTOR shadowPos2 = VAdd(position, VGet(0.0f, Chara::Base::SHADOW_OFFSET_Y_LOW, 0.0f));
		DrawCone3D(shadowPos1, shadowPos2, status.width / Chara::Base::SHADOW_WIDTH_DIVISOR, System::Window::CIRCLE_DIVNUM, GetColor(Global::Palette::BLACK.r, Global::Palette::BLACK.g, Global::Palette::BLACK.b), GetColor(Global::Palette::BLACK.r, Global::Palette::BLACK.g, Global::Palette::BLACK.b), TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}

	DrawCapsule3D(bottom, bodyTop, bodyRad, System::Window::CIRCLE_DIVNUM, color, color, fillFlag);
	DrawSphere3D(headPos, headRad, System::Window::CIRCLE_DIVNUM, color, color, fillFlag);

	if (sniper) {
		VECTOR forward = VGet(0.0f, 0.0f, 1.0f);
		if (target) {
			forward = VNorm(VSub(target->GetPos(), position));
			forward.y = 0.0f;
			if (VSize(forward) < System::Collision::MIN_DIST_SQUARED) forward = VGet(0.0f, 0.0f, 1.0f);
		}
		VECTOR right = VNorm(VCross(VGet(0.0f, 1.0f, 0.0f), forward));
		VECTOR up = VNorm(VCross(forward, right));

		VECTOR drawPos = VAdd(position, VGet(0.0f, status.height * 0.5f, 0.0f));
		float weaponHeight = (status.height * 0.5f) - bodyRad;
		drawPos = VAdd(drawPos, VScale(leanMax, weaponHeight / headHeight));
		sniper->Draw(drawPos, forward, right, up, false, false);
	}

	if (targetingTimer > 0.0f) {
		if (sniper && target) {
			VECTOR gunOffset = VAdd(sniper->GetSpec().muzzleOffset, VGet(0.0f, currentEyeHeight, 0.0f));
			VECTOR s = VAdd(position, gunOffset);
			VECTOR e = VAdd(target->GetPos(), VGet(0.0f, target->GetCurrentHeight() * 0.5f, 0.0f));

			float progress = targetingTimer / Chara::Sniper::TARGET_TIME;

			int outerColor;
			int innerColor = GetColor(Global::Palette::WHITE.r, Global::Palette::WHITE.g, Global::Palette::WHITE.b);
			float outerRad = 0.0f;
			float innerRad = 0.0f;
			int outerAlpha = 0;

			if (progress < Chara::Sniper::INDICATOR_PHASE1_PROGRESS_RATIO) {
				float t = progress / Chara::Sniper::INDICATOR_PHASE1_PROGRESS_RATIO;
				outerColor = GetColor(Chara::Sniper::INDICATOR_OUTER_COLOR_PHASE1.r, Chara::Sniper::INDICATOR_OUTER_COLOR_PHASE1.g, Chara::Sniper::INDICATOR_OUTER_COLOR_PHASE1.b);
				outerRad = Chara::Sniper::INDICATOR_OUTER_RADIUS_PHASE1;
				innerRad = Chara::Sniper::INDICATOR_INNER_RADIUS_PHASE1;
				outerAlpha = static_cast<int>(Chara::Sniper::INDICATOR_OUTER_ALPHA_PHASE1_MAX * t);
			}
			else {
				float t = (progress - Chara::Sniper::INDICATOR_PHASE1_PROGRESS_RATIO) / Chara::Sniper::INDICATOR_PHASE2_PROGRESS_RATIO;
				int r = Chara::Sniper::INDICATOR_OUTER_COLOR_PHASE2_BASE.r;
				int g = static_cast<int>(Chara::Sniper::INDICATOR_OUTER_COLOR_PHASE2_BASE.g * (1.0f - t));
				int b = Chara::Sniper::INDICATOR_OUTER_COLOR_PHASE2_BASE.b;
				outerColor = GetColor(r, g, b);

				outerRad = Chara::Sniper::INDICATOR_OUTER_RADIUS_PHASE2_BASE + sinf(t * DX_PI_F * Chara::Sniper::INDICATOR_WAVE_FREQ_MULT) * Chara::Sniper::INDICATOR_OUTER_RADIUS_WAVE_AMPL;
				innerRad = Chara::Sniper::INDICATOR_INNER_RADIUS_PHASE2;
				outerAlpha = Chara::Sniper::INDICATOR_OUTER_ALPHA_PHASE2;
			}

			SetWriteZBuffer3D(FALSE);

			SetDrawBlendMode(DX_BLENDMODE_ALPHA, outerAlpha);
			DrawCapsule3D(s, e, outerRad, Chara::Sniper::INDICATOR_CAPSULE_SEGMENTS, outerColor, outerColor, TRUE);

			SetDrawBlendMode(DX_BLENDMODE_ADD, 255);
			DrawCapsule3D(s, e, innerRad, Chara::Sniper::INDICATOR_CAPSULE_SEGMENTS, innerColor, innerColor, TRUE);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			SetWriteZBuffer3D(TRUE);
		}
	}
}