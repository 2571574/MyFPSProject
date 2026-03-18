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
	melee = ENEMY_GUN::MELEE;
}

void MeleeEnemy::Update() {
	float dt = Time::GetIns().GetDelta();
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

	switch (state) {
	case MeleeState::IDLE: {
		if (attackTimer > 0.0f)attackTimer -= dt;
		VECTOR moveTarget = UpdateNavigation(target, dt);

		VECTOR dir = VNorm(VSub(moveTarget, position));
		dir.y = 0.0f;

		VECTOR targetPos = target->GetPos();
		float distToTarget = VSize(VSub(targetPos, position));
		if (distToTarget > melee.range* 0.9f) {
			ApplyMovement(dir, stageHandle);
		}
		else {
			ApplyMovement(VGet(0, 0, 0), stageHandle);
		}

		if (distToTarget <= melee.range && std::abs(target->GetPos().y - position.y) <= currentHeight && attackTimer <= 0.0f) {
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
			attackTimer = 1.0f / melee.fireRate;
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

	DrawCapsule3D(bottom, bodyTop, bodyRad, 16, GetColor(255, 255, 0), GetColor(255, 255, 0), true);
	VECTOR headPos = VAdd(cPos, VGet(0.0f, currentEyeHeight, 0.0f));
	DrawSphere3D(headPos, headRad, 16, GetColor(255, 255, 0), GetColor(255, 255, 0), true);
}

void MeleeEnemy::Action() {
	VECTOR toTarget = VSub(target->GetPos(), position);
	toTarget.y = 0.0f;
	toTarget = VNorm(toTarget);

	VECTOR attackPos = VAdd(position, VScale(toTarget, 1.0f));

	CollisionManager::GetIns().ProcessExplotion(attackPos, melee.explodeArea, melee.damage, melee.knockbackP, false, GetID(), WeaponID::ENEMY_KNIFE,melee.friendlyFire);
}