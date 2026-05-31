#include "MeleeEnemy.h"
#include "Param/Global.h"
#include "Param/Chara.h"
#include "Param/System.h"
#include "Player.h"
#include "Time.h"
#include "EnemyManager.h"
#include "SoundManager.h"

#include <cmath>

MeleeEnemy::MeleeEnemy(VECTOR pos, Player* target)
	:Enemy(pos, CHARA_STATUS::MELEE_ENEMY, target, ENEMYTYPE::MELEE)
	, attackTimer(0.0f) {
	melee = std::make_unique<Weapon>(ENEMY_GUN::MELEE);
}

void MeleeEnemy::Update() {
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

	if (target == nullptr) return;

	if (melee) {
		melee->Update(*this);
	}

	float range = 0.0f;
	float fireRate = 0.0f;

	if (melee) {
		range = melee->GetSpec().range;
		fireRate = melee->GetSpec().fireRate;
	}

	switch (state) {

		//プレイヤーに向かって移動
	case MeleeState::IDLE: {
		if (attackTimer > 0.0f)attackTimer -= dt;

		VECTOR moveTarget = UpdateNavigation(target, dt);
		VECTOR dir = VNorm(VSub(moveTarget, position));
		dir.y = 0.0f;

		VECTOR targetPos = target->GetPos();
		float distToTarget = VSize(VSub(targetPos, position));

		if (distToTarget > range * Chara::Melee::MOVE_STOP_RANGE_RATIO) {
			ApplyMovement(dir, stageHandle);
		}

		else {
			ApplyMovement(VGet(0.0f, 0.0f, 0.0f), stageHandle);
		}

		if (distToTarget <= range && std::abs(target->GetPos().y - position.y) <= currentHeight && attackTimer <= 0.0f) {
			state = MeleeState::ATTACK_WIND;
			stateTimer = Chara::Melee::ATTACK_WINDUP_TIME;
		}
		break;
	}
	
	//振りかぶって攻撃
	case MeleeState::ATTACK_WIND: {
		ApplyMovement(VGet(0.0f, 0.0f, 0.0f), stageHandle);

		stateTimer -= dt;

		if (stateTimer <= 0.0f) {
			Action();
			state = MeleeState::ATTACK_RECOVERY;
			stateTimer = Chara::Melee::ATTACK_RECOVERY_TIME;
		}
		break;
	}

	//攻撃後の硬直
	case MeleeState::ATTACK_RECOVERY: {
		ApplyMovement(VGet(0.0f, 0.0f, 0.0f), stageHandle);

		stateTimer -= dt;

		if (stateTimer <= 0.0f) {
			state = MeleeState::IDLE;
			attackTimer = 1.0f / fireRate;
		}
		break;
	}
	}

	UpdateFootstep();
}

void MeleeEnemy::Draw() {
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

	int color = GetColor(Chara::Melee::COLOR_NORMAL.r, Chara::Melee::COLOR_NORMAL.g, Chara::Melee::COLOR_NORMAL.b);
	if (onHitFlashTimer > 0.0f) color = GetColor(Global::Palette::WHITE.r, Global::Palette::WHITE.g, Global::Palette::WHITE.b);

	int fillFlag = nowSpawned ? FALSE : TRUE;

	//影
	if (!nowSpawned) {
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, Chara::Base::SHADOW_ALPHA);
		VECTOR shadowPos1 = VAdd(position, VGet(0.0f, Chara::Base::SHADOW_OFFSET_Y_HIGH, 0.0f));
		VECTOR shadowPos2 = VAdd(position, VGet(0.0f, Chara::Base::SHADOW_OFFSET_Y_LOW, 0.0f));
		DrawCone3D(shadowPos1, shadowPos2, status.width / Chara::Base::SHADOW_WIDTH_DIVISOR, System::Window::CIRCLE_DIVNUM, GetColor(Global::Palette::BLACK.r, Global::Palette::BLACK.g, Global::Palette::BLACK.b), GetColor(Global::Palette::BLACK.r, Global::Palette::BLACK.g, Global::Palette::BLACK.b), TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}

	//カプセル
	DrawCapsule3D(bottom, bodyTop, bodyRad, System::Window::CIRCLE_DIVNUM, color, color, fillFlag);
	DrawSphere3D(headPos, headRad, System::Window::CIRCLE_DIVNUM, color, color, fillFlag);

	//武器
	if (melee) {

		VECTOR forward = VGet(0.0f, 0.0f, 1.0f);

		if (target) {
			forward = VNorm(VSub(target->GetPos(), position));
			forward.y = 0.0f;

			if (VSize(forward) < System::Collision::MIN_DIST_SQUARED) forward = VGet(0.0f, 0.0f, 1.0f);
		}

		VECTOR right = VNorm(VCross(VGet(0.0f, 1.0f, 0.0f), forward));
		VECTOR up = VNorm(VCross(forward, right));


		float swingAngle = 0.0f;

		//振り下ろし
		if (state == MeleeState::ATTACK_WIND) {
			float t = 1.0f - (stateTimer / Chara::Melee::ATTACK_WINDUP_TIME);
			swingAngle = t * -Chara::Melee::SWING_90_DEG;
		}

		//攻撃後の硬直
		else if (state == MeleeState::ATTACK_RECOVERY) {
			float t = 1.0f - (stateTimer / Chara::Melee::ATTACK_RECOVERY_TIME);

			if (t < Chara::Melee::RECOVERY_PHASE1_RATIO) {
				float p = t / Chara::Melee::RECOVERY_PHASE1_RATIO;
				swingAngle = -Chara::Melee::SWING_90_DEG * (1.0f - p) + Chara::Melee::SWING_QUARTER_TURN * p;
			}

			else {
				float p = (t - Chara::Melee::RECOVERY_PHASE1_RATIO) / Chara::Melee::RECOVERY_PHASE2_RATIO;
				swingAngle = Chara::Melee::SWING_QUARTER_TURN * (1.0f - p);
			}
		}

		MATRIX rot = MGetRotAxis(right, swingAngle);
		VECTOR animForward = VTransform(forward, rot);
		VECTOR animUp = VTransform(up, rot);

		VECTOR drawPos = VAdd(position, VGet(0.0f, status.height * 0.5f, 0.0f));
		float weaponHeight = (status.height * 0.5f) - bodyRad;
		drawPos = VAdd(drawPos, VScale(leanMax, weaponHeight / headHeight));
		melee->Draw(drawPos, animForward, right, animUp, false, false);
	}
}

void MeleeEnemy::Action() {
	VECTOR toTarget = VSub(target->GetPos(), position);
	toTarget.y = 0.0f;
	toTarget = VNorm(toTarget);

	VECTOR attackPos = VAdd(position, VScale(toTarget, 1.0f));
	auto spec = melee->GetSpec();

	//攻撃判定
	CollisionManager::GetIns().ProcessExplosion(attackPos, spec.explodeArea, spec.damage, spec.knockbackP, false, GetID(), WeaponID::ENEMY_KNIFE, spec.friendlyFire);

	SoundManager::GetIns().Play3DSE("Resource/Sound/melee.wav", attackPos, Chara::Melee::MELEE_ATTACK_SOUND_RADIUS);
}