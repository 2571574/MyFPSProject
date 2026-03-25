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
		ApplyMovement(VGet(0, 0, 0), stageHandle);
	}

	if (CheckLineSight(target,target->GetCurrentEyeHeight())) {
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
	int color = GetColor(0, 0, 255);
	if (onHitFlashTimer > 0.0f)color = GetColor(255, 255, 255);

	int fillFlag = nowSpawned ? FALSE : TRUE;

	if (!nowSpawned) {
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 100);
		VECTOR shadowPos1 = VAdd(position, VGet(0.0f, 0.02f, 0.0f));
		VECTOR shadowPos2 = VAdd(position, VGet(0.0f, 0.01f, 0.0f));
		DrawCone3D(shadowPos1, shadowPos2, status.width / 1.5f, 16, GetColor(0, 0, 0), GetColor(0, 0, 0), TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}

	DrawCapsule3D(bottom, bodyTop, bodyRad, CIRCLE_DIVNUM, color, color, fillFlag);
	DrawSphere3D(headPos, headRad, CIRCLE_DIVNUM, color, color, fillFlag);

	if (rifle) {
		VECTOR forward = VGet(0, 0, 1);
		if (target) {
			forward = VNorm(VSub(target->GetPos(), position));
			forward.y = 0.0f;
			if (VSize(forward) < 0.01f) forward = VGet(0, 0, 1);
		}
		VECTOR right = VNorm(VCross(VGet(0, 1, 0), forward));
		VECTOR up = VNorm(VCross(forward, right));

		VECTOR drawPos = VAdd(position, VGet(0, status.height * 0.5f, 0));
		float weaponHeight = (status.height * 0.5f) - bodyRad;
		drawPos = VAdd(drawPos, VScale(leanMax, weaponHeight / headHeight));
		rifle->Draw(drawPos, forward, right, up, false, false);
	}
}