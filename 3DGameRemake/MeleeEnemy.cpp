#include "MeleeEnemy.h"
#include "Player.h"
#include "Time.h"
#include <cmath>
#include "Parameter.h"
#include "EnemyManager.h"
MeleeEnemy::MeleeEnemy(VECTOR pos, Player* target) :Enemy(pos, CHARA_STATUS::MELEE_ENEMY, target,ENEMYTYPE::MELEE), attackTimer(0.0f), range(3.0f), moveSpeed(3.0f) {}

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
		if (distToTarget > range * 0.9f) {
			ApplyMovement(dir, stageHandle);
		}
		else {
			ApplyMovement(VGet(0, 0, 0), stageHandle);
		}

		if (distToTarget <= range && std::abs(target->GetPos().y - position.y) <= currentHeight && attackTimer <= 0.0f) {
			state = MeleeState::ATTACK_WIND;
			stateTimer = 0.3f;
		}
		break;
	}

	case MeleeState::ATTACK_WIND: {
		ApplyMovement(VGet(0, 0, 0), stageHandle);

		stateTimer -= dt;
		if (stateTimer <= 0.0f) {
			Action();
			state = MeleeState::ATTACK_RECOVERY;
			stateTimer = 0.5f;
		}
		break;
	}

	case MeleeState::ATTACK_RECOVERY: {
		ApplyMovement(VGet(0.0f, 0.0f, 0.0f), stageHandle);
		stateTimer -= dt;
		if (stateTimer <= 0.0f) {
			state = MeleeState::IDLE;
			attackTimer = 1.0f;
		}
		break;
	}
	}
}



void MeleeEnemy::Draw() {
	float bodyRad = status.width / 2.0f;
	VECTOR bottom = VAdd(position, VGet(0.0f, bodyRad, 0.0f));
	VECTOR top = VAdd(position, VGet(0, currentHeight - bodyRad, 0));
	DrawCapsule3D(bottom, top, bodyRad, CIRCLE_DIVNUM, GetColor(255, 125, 0), GetColor(255, 125, 0), TRUE);

	SetUseZBuffer3D(false);
	VECTOR cPos = GetPos();
	float headRadius = 0.25f;
	VECTOR bodyTop = VAdd(cPos, VGet(0.0f, status.height - bodyRad, 0.0f));
	float bodyRadius = status.width / 2.0f;

	DrawCapsule3D(bottom, bodyTop, bodyRadius, 16, GetColor(0, 255, 0), GetColor(0, 255, 0), FALSE);
	VECTOR headPos = VAdd(cPos, VGet(0.0f, currentEyeHeight, 0.0f));
	DrawSphere3D(headPos, headRadius, 16, GetColor(255, 0, 0), GetColor(255, 0, 0), FALSE);
	SetUseZBuffer3D(true);
}

void MeleeEnemy::Action() {
	VECTOR toTarget = VSub(target->GetPos(), position);
	toTarget.y = 0.0f;
	toTarget = VNorm(toTarget);

	VECTOR attackPos = VAdd(position, VScale(toTarget, 1.0f));

	float attackRadius = 2.5f;
	int damage = 1;
	float knockbackPower = 0.0f;
	CollisionManager::GetIns().ProcessExplotion(attackPos, attackRadius, damage, knockbackPower, false, GetID(), WeaponID::ENEMY_KNIFE);
	Debug::Log("EnemyAttack");
}