#include "SniperEnemy.h"
#include "Player.h"
#include "Status.h"
#include "Time.h"

SniperEnemy::SniperEnemy(VECTOR pos, Player* target) : Enemy(pos, CHARA_STATUS::SNIPER_ENEMY, target,ENEMYTYPE::SNIPER), targetingTimer(0.0f) {
	sniper = std::make_unique<Weapon>(ENEMY_GUN::SNIPER);
	attackDist = sniper->GetSpec().range * 0.8f;
	escapeDist = sniper->GetSpec().range * 0.4f;
}

void SniperEnemy::Update() {
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

	if (sniper)sniper->Update();
	if (target == nullptr)return;

	VECTOR moveTarget = UpdateNavigation(target, dt);
	float distToPlayer = VSize(VSub(target->GetPos(), position));
	bool hasLos = CheckLineSight(target, target->GetCurrentHeight() * 0.5f);
	VECTOR moveDir = VGet(0, 0, 0);

	if (distToPlayer < escapeDist) {
		VECTOR escapeDir = VNorm(VSub(position, target->GetPos()));

		if(CheckPathSafety(VAdd(position,VScale(escapeDir,2.0f)))){
			moveDir = escapeDir;
		}
		else {
			moveDir = VGet(0.0f, 0.0f, 0.0f);
		}
	}
	else if (distToPlayer > attackDist || !hasLos) {
		moveDir = VNorm(VSub(moveTarget, position));
	}

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
		sniper->Reload();
	}
	if (sniper->CanFire() && CheckLineSight(target,target->GetCurrentHeight() * 0.5f) && distToPlayer <= sniper->GetSpec().range) {
		targetingTimer += dt;
		if (target) {
			target->AddTargeted(position);
		}
		if (targetingTimer >= TARGET_TIME) {
			Action();
			targetingTimer = 0.0f;
		}
	}
	else {
		targetingTimer = 0.0f;
	}
}

void SniperEnemy::Action() {
	if (!sniper || sniper->Reloading())return;

	VECTOR s = VAdd(position, VGet(0, status.eyeHeight, 0));
	VECTOR e = VAdd(target->GetPos(), VGet(0.0f, target->GetCurrentHeight() * 0.5f, 0.0f));
	VECTOR fireDir = VNorm(VSub(e, s));

	sniper->Fire(*this, fireDir);
	
}

void SniperEnemy::Draw() {
	VECTOR cPos = GetPos();
	float bodyRad = status.width / 2.0f;
	float headRad = bodyRad * 0.5f;
	if (headRad < 0.15f)headRad = 0.15f;
	VECTOR bottom = VAdd(position, VGet(0.0f, bodyRad, 0.0f));
	float neck = status.eyeHeight - headRad;
	VECTOR bodyTop = VAdd(cPos, VGet(0.0f, neck - bodyRad, 0.0f));

	DrawCapsule3D(bottom, bodyTop, bodyRad, 16, GetColor(0, 255, 0), GetColor(0, 255, 0), true);
	VECTOR headPos = VAdd(cPos, VGet(0.0f, currentEyeHeight, 0.0f));
	DrawSphere3D(headPos, headRad, 16, GetColor(0, 255, 0), GetColor(0, 255, 0), true);
	
	if (targetingTimer > 0.0f) {
		if (sniper) {
			VECTOR gunOffset =VAdd(sniper->GetSpec().muzzleOffset,VGet(0.0f,currentEyeHeight,0.0f));

			VECTOR s = VAdd(position, gunOffset);
			VECTOR e = VAdd(target->GetPos(), VGet(0.0f, target->GetCurrentHeight() * 0.5f, 0.0f));

			Debug::Watch("laser.y", e.y);
			int a = (int)((targetingTimer / TARGET_TIME) * 255);
			DrawLine3D(s, e, GetColor(a, 0, 0));
		}
	}
}