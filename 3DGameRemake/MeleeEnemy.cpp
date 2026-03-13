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
		ApplyMovement(dir, stageHandle);
	}
	else {
		ApplyMovement(VGet(0, 0, 0),stageHandle);
	}
	VECTOR targetPos = target->GetPos();
	float distToTarget = VSize(VSub(targetPos, position));
	if (distToTarget <= range && std::abs(targetPos.y - position.y) <= currentHeight && attackTimer <= 0.0f) {
		Action();
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
	target->TakeDamage(1,WeaponID::ENEMY_KNIFE);

	attackTimer = 0.5f;

	Debug::Log("EnemyAttack");
}