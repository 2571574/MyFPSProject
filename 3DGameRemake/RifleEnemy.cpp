#include "RifleEnemy.h"
#include "Player.h"
#include "Time.h"
#include "EnemyManager.h"
#include "Status.h"

namespace {
	constexpr float STOP_DISTANCE_RATIO = 0.6f;
}
RifleEnemy::RifleEnemy(VECTOR pos,Player*target)
	: Enemy(pos,CHARA_STATUS::RIFLE_ENEMY,target,ENEMYTYPE::RIFLE){
	
	rifle = std::make_unique<Weapon>(ENEMY_GUN::RIFLE);
	range = rifle->GetSpec().range;
	stopDist = range * STOP_DISTANCE_RATIO;
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
	VECTOR targetEyePos = VAdd(target->GetPos(), VGet(0.0f, target->GetCurrentEyeHeight(), 0.0f));
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
	VECTOR cPos = GetPos();
	float bodyRad = status.width / 2.0f;
	float headRad = bodyRad * 0.5f;
	if (headRad < 0.15f)headRad = 0.15f;
	VECTOR bottom = VAdd(position, VGet(0.0f, bodyRad, 0.0f));
	float neck = status.eyeHeight - headRad;
	VECTOR bodyTop = VAdd(cPos, VGet(0.0f, neck - bodyRad, 0.0f));

	DrawCapsule3D(bottom, bodyTop, bodyRad, 16, GetColor(0, 0, 255), GetColor(0, 0, 255), true);
	VECTOR headPos = VAdd(cPos, VGet(0.0f, currentEyeHeight, 0.0f));
	DrawSphere3D(headPos, headRad, 16, GetColor(0, 0, 255), GetColor(0, 0, 255), true);

}