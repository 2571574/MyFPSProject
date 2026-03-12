#include "RifleEnemy.h"
#include "Player.h"
#include "Time.h"
#include "EnemyManager.h"
#include "Status.h"

RifleEnemy::RifleEnemy(VECTOR pos,Player*target)
	: Enemy(pos,CHARA_STATUS::RIFLE_ENEMY,target,ENEMYTYPE::RIFLE)
	,moveSpeed(2.0f){
	
	rifle = std::make_unique<Weapon>(ENEMY_GUN::RIFLE);
	range = rifle->GetSpec().range;
	stopDist = range * 0.6f;
}

void RifleEnemy::Update() {
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

	if (rifle) {
		rifle->Update();
	}
	if (target == nullptr) return;

	
	VECTOR moveTarget = UpdateNavigation(target, dt);

	VECTOR dir = VNorm(VSub(moveTarget, position));
	dir.y = 0.0f;

	if (VSize(VSub(target->GetPos(), position)) > stopDist) {
		ApplyMovement(dir, stageHandle);
	}
	else {
		ApplyMovement(VGet(0, 0, 0), stageHandle);
	}

	if (CheckLineSight(target,target->GetCurrentEyeHeight())) {
		Action();
	}
}

void RifleEnemy::Action() {
	VECTOR eyePos = VAdd(position, VGet(0.0f, status.eyeHeight, 0.0f));
	VECTOR targetEyePos = VAdd(target->GetPos(), VGet(0.0f, target->GetStatus().height * 0.8f, 0.0f));
	VECTOR fireDir = VNorm(VSub(targetEyePos, eyePos));
	if (rifle) {
		if (rifle->GetAmmo() <= 0) {
			rifle->Reload();
		}
		if (!rifle->Reloading()) {
			rifle->Fire(*this, fireDir);
		}
	}
}


void RifleEnemy::Draw() {
	float bodyRad = status.width / 2.0f;
	VECTOR bottom = VAdd(position, VGet(0.0f, bodyRad, 0.0f));
	VECTOR top = VAdd(position, VGet(0, currentHeight - bodyRad, 0));
	DrawCapsule3D(bottom, top, bodyRad, 16, GetColor(0, 0, 255), GetColor(0, 0, 255), true);

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