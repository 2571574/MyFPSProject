#include "MeleeEnemy.h"
#include "Player.h"
#include "Time.h"
#include <cmath>
#include "Parameter.h"
#include "EnemyManager.h"

namespace {
	constexpr float ATTACK_WINDUP_TIME = 0.3f;
	constexpr float ATTACK_RECOVERY_TIME = 0.5f;
}

MeleeEnemy::MeleeEnemy(VECTOR pos, Player* target)
	:Enemy(pos, CHARA_STATUS::MELEE_ENEMY, target,ENEMYTYPE::MELEE)
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
		melee->Update();
	}

	float range = 0.0f;
	float fireRate = 0.0f;
	if (melee) {
		range = melee->GetSpec().range;
		fireRate = melee->GetSpec().fireRate;
	}
	switch (state) {
	case MeleeState::IDLE: {
		if (attackTimer > 0.0f)attackTimer -= dt;
		VECTOR moveTarget = UpdateNavigation(target, dt);

		VECTOR dir = VNorm(VSub(moveTarget, position));
		dir.y = 0.0f;

		VECTOR targetPos = target->GetPos();
		float distToTarget = VSize(VSub(targetPos, position));
		if (distToTarget > range* 0.9f) {
			ApplyMovement(dir, stageHandle);
		}
		else {
			ApplyMovement(VGet(0, 0, 0), stageHandle);
		}

		if (distToTarget <= range && std::abs(target->GetPos().y - position.y) <= currentHeight && attackTimer <= 0.0f) {
			state = MeleeState::ATTACK_WIND;
			stateTimer = ATTACK_WINDUP_TIME;
		}
		break;
	}

	case MeleeState::ATTACK_WIND: {
		ApplyMovement(VGet(0, 0, 0), stageHandle);

		stateTimer -= dt;
		if (stateTimer <= 0.0f) {
			Action();
			state = MeleeState::ATTACK_RECOVERY;
			stateTimer = ATTACK_RECOVERY_TIME;
		}
		break;
	}

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
}



void MeleeEnemy::Draw() {
	VECTOR cPos = GetPos();
	float bodyRad = status.width / 2.0f;
	float headRad = bodyRad * 0.5f;
	if (headRad < 0.15f)headRad = 0.15f;
	VECTOR bottom = VAdd(position, VGet(0.0f, bodyRad, 0.0f));
	float neck = status.eyeHeight - headRad;
	VECTOR bodyTop = VAdd(cPos, VGet(0.0f, neck - bodyRad, 0.0f));
	VECTOR headPos = VAdd(cPos, VGet(0.0f, currentEyeHeight, 0.0f));

	VECTOR leanMax = VGet(velocity.x, 0.0f, velocity.z);
	constexpr float LEAN_FACTOR = 1.5f;
	leanMax = VScale(leanMax, LEAN_FACTOR);

	float headHeight = currentEyeHeight - bodyRad;
	float bodyTopHeight = (neck - bodyRad) - bodyRad;

	VECTOR bodyTopLean = VScale(leanMax, bodyTopHeight / headHeight);

	bodyTop = VAdd(bodyTop, bodyTopLean);
	headPos = VAdd(headPos, leanMax);

	int color = GetColor(255, 255, 0);
	if (onHitFlashTimer > 0.0f) color = GetColor(255, 255, 255);

	DrawCapsule3D(bottom, bodyTop, bodyRad, 16, color, color, true);
	DrawSphere3D(headPos, headRad, 16, color, color, true);


	if (melee) {
		VECTOR forward = VGet(0, 0, 1);
		if (target) {
			forward = VNorm(VSub(target->GetPos(), position));
			forward.y = 0.0f;
			if (VSize(forward) < 0.01f) forward = VGet(0, 0, 1);
		}
		VECTOR right = VNorm(VCross(VGet(0, 1, 0), forward));
		VECTOR up = VNorm(VCross(forward, right));

		float swingAngle = 0.0f;
		if (state == MeleeState::ATTACK_WIND) {
		
			float t = 1.0f - (stateTimer / 0.3f); 
			swingAngle = t * -(DX_PI_F / 2.0f);
		}
		else if (state == MeleeState::ATTACK_RECOVERY) {
			float t = 1.0f - (stateTimer / 0.5f); 
			if (t < 0.3f) {
				float p = t / 0.3f;
				swingAngle = -(DX_PI_F / 2.0f) * (1.0f - p) + (DX_PI_F / 4.0f) * p;
			}
			else {
				float p = (t - 0.3f) / 0.7f;
				swingAngle = (DX_PI_F / 4.0f) * (1.0f - p);
			}
		}

		// 右方向ベクトルを軸にして上下に回転させる
		MATRIX rot = MGetRotAxis(right, swingAngle);
		VECTOR animForward = VTransform(forward, rot);
		VECTOR animUp = VTransform(up, rot);

		// 体の中心付近(手元の高さ)を基準に描画
		VECTOR drawPos = VAdd(position, VGet(0, status.height * 0.5f, 0));
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
	CollisionManager::GetIns().ProcessExplotion(attackPos, spec.explodeArea, spec.damage, spec.knockbackP, false, GetID(), WeaponID::ENEMY_KNIFE,spec.friendlyFire);
	Debug::Log("EnemyAttack");
}