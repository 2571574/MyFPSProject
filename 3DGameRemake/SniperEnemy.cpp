#include "SniperEnemy.h"
#include "Player.h"
#include "Status.h"
#include "Time.h"

SniperEnemy::SniperEnemy(VECTOR pos, Player* target) : Enemy(pos, CHARA_STATUS::SNIPER_ENEMY, target), targetingTimer(0.0f) {
	sniper = std::make_unique<Weapon>(ENEMY_GUN::SNIPER);
	attackDist = sniper->GetSpec().range * 0.8f;
	escapeDist = sniper->GetSpec().range * 0.4f;
}

void SniperEnemy::Update() {
	if (hp <= 0) {
		alive = false;
		return;
	}

	float dt = Time::GetIns().GetDelta();
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
	ApplyMovement(moveDir, dt);

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
	VECTOR top = VAdd(position, VGet(0.0f, currentHeight, 0.0f));
	DrawCapsule3D(position, top, status.width, 16, GetColor(0, 255, 0), GetColor(0, 255, 0), TRUE);

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