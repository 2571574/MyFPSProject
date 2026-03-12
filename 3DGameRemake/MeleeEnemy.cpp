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

	if (attackTimer > 0.0f) {
		attackTimer -= dt;
	}
	if (target == nullptr) return;
	
	VECTOR moveTarget = UpdateNavigation(target, dt);

	VECTOR dir = VNorm(VSub(moveTarget, position));
	dir.y = 0.0f;

	if (VSize(VSub(target->GetPos(), position)) > range * 0.9f) {
		ApplyMovement(dir, dt);
	}
	else {
		ApplyMovement(VGet(0, 0, 0), dt);
	}
	VECTOR targetPos = target->GetPos();
	float distToTarget = VSize(VSub(targetPos, position));
	if (distToTarget <= range && std::abs(targetPos.y - position.y) <= currentHeight && attackTimer <= 0.0f) {
		Action();
	}
}



void MeleeEnemy::Draw() {
	VECTOR top = VAdd(position, VGet(0, currentHeight, 0));
	DrawCapsule3D(position, top, status.width, CIRCLE_DIVNUM, GetColor(255, 125, 0), GetColor(255, 125, 0), TRUE);
}

void MeleeEnemy::Action() {
	target->TakeDamage(1);

	attackTimer = 0.5f;

	Debug::Log("EnemyAttack");
}