#include "Enemy.h"
#include "EnemyManager.h"
#include "Time.h"
#include "SoundManager.h"
#include "Param/Global.h"
#include "Param/Chara.h"
#include "Param/System.h"

#include <cmath>

Enemy::Enemy(VECTOR pos, const CharacterStatus& status, Player* _target, ENEMYTYPE type)
	: Character(pos, status)
	, target(_target)
	, currentNodeID(0)
	, isDirectPathSafe(false)
	, type(type)
	, nowSpawned(true)
	, spawnedTimer(Chara::EnemyCommon::SPAWN_PROCESS_TIME)
	, onHitFlashTimer(0.0f)
	, moveDistance(0.0f) {
	pathUpdateTimer = (GetRand(Chara::EnemyCommon::PATH_UPDATE_RANDOM) / Global::Math::PERCENT_MAX);
}

void Enemy::OnHit(int damage, WeaponID id) {
	if (!alive || nowSpawned) return;
	TakeDamage(damage, id);
	onHitFlashTimer = Chara::EnemyCommon::HIT_FLASH_DURATION;
}

VECTOR Enemy::GetNextNodeID()const {
	if (currentPath.empty() || currentNodeID >= currentPath.size()) {
		return VGet(0.0f, 0.0f, 0.0f);
	}
	return currentPath[currentNodeID];
}

bool Enemy::CheckLineSight(const Character* target, float height)const {
	VECTOR myEye = VAdd(position, VGet(0.0f, currentEyeHeight, 0.0f));
	VECTOR targetEye = VAdd(target->GetPos(), VGet(0.0f, height, 0.0f));
	MV1_COLL_RESULT_POLY hitResult = MV1CollCheck_Line(stageHandle, -1, myEye, targetEye);
	return hitResult.HitFlag == 0; //ヒットしていないなら見えている
}

bool Enemy::CheckPathSafety(VECTOR targetPos)const {
	VECTOR toTarget = VSub(targetPos, position);
	float distXZ = VSize(VGet(toTarget.x, 0.0f, toTarget.z));
	int sample = static_cast<int>(distXZ / Chara::EnemyCommon::PATH_SAFETY_SAMPLE_STEP);
	float prevY = position.y;

	for (int i = 1; i <= sample; i++) {
		float t = (float)i / sample;
		VECTOR checkPos = VAdd(position, VScale(toTarget, t));

		VECTOR s = VGet(checkPos.x, prevY + Chara::EnemyCommon::PATH_SAFETY_RAY_HEIGHT, checkPos.z);
		VECTOR e = VGet(checkPos.x, prevY - Chara::EnemyCommon::PATH_SAFETY_RAY_HEIGHT, checkPos.z);
		MV1_COLL_RESULT_POLY ground = MV1CollCheck_Line(stageHandle, -1, s, e);
		if (ground.HitFlag == 0 || ground.Normal.y < Chara::EnemyCommon::PATH_SAFETY_NORMAL_MIN || std::abs(ground.HitPosition.y - prevY)> Chara::EnemyCommon::PATH_SAFETY_HEIGHT_DIFF_MAX) {
			return false;
		}
		prevY = ground.HitPosition.y;
	}
	float heightDiff = targetPos.y - position.y;
	if (std::abs(heightDiff) > Chara::EnemyCommon::PATH_SAFETY_TOTAL_HEIGHT_DIFF) {
		return false;
	}
	return true;
}

VECTOR Enemy::UpdateNavigation(const Character* target, float dt) {
	bool hasLOS = CheckLineSight(target, target->GetCurrentEyeHeight());

	isDirectPathSafe = hasLOS && CheckPathSafety(target->GetPos());

	pathUpdateTimer -= dt;
	if (pathUpdateTimer <= 0.0f) {
		if (!isDirectPathSafe) {
			SetPath(EnemyManager::GetIns().CalculatePath(position, target->GetPos()));
		}
		pathUpdateTimer = Chara::EnemyCommon::PATH_UPDATE_BASE_INTERVAL + (GetRand(Chara::EnemyCommon::PATH_UPDATE_RANDOM) / Global::Math::PERCENT_MAX);
	}

	VECTOR moveTarget = target->GetPos();
	if (!isDirectPathSafe && HasPath()) {
		moveTarget = GetNextNodeID();
		VECTOR toNode = VSub(moveTarget, position);
		toNode.y = 0.0f;
		if (VSize(toNode) < Chara::EnemyCommon::PATH_NODE_REACHED_DIST) {
			AdvancePathIndex();
			if (HasPath()) {
				moveTarget = GetNextNodeID();
			}
		}
	}
	return moveTarget;
}

bool Enemy::CheckFall() const { return position.y < Chara::EnemyCommon::FALL_DEATH_Y; }

void Enemy::UpdateFootstep() {
	if (!onGround || !alive || nowSpawned) return;

	float dt = Time::GetIns().GetDelta();
	float dt60 = dt * Global::Math::FPS_BASE;

	float speed = VSize(VGet(velocity.x, 0.0f, velocity.z));

	if (speed > Chara::Base::MOVEMENT_MIN) {
		moveDistance += speed * dt60;

		if (moveDistance >= Chara::EnemyCommon::STEP_LENGTH) {
			SoundManager::GetIns().Play3DSE("Resource/Sound/footstep.ogg", position, Chara::EnemyCommon::FOOTSTEP_SOUND_RADIUS);
			moveDistance -= Chara::EnemyCommon::STEP_LENGTH;
		}
	}
	else {
		moveDistance = 0.0f;
	}
}