#include "Enemy.h"
#include "EnemyManager.h"

#include <cmath>

namespace {
	constexpr float SPAWN_INVINCIBLE_TIME = 1.5f;
	constexpr float PATH_UPDATE_BASE_TIME = 0.5f;
	constexpr int PATH_UPDATE_RANDOM = 50;
	constexpr float PATH_NODE_REACHED_DIST = 1.0f;

	constexpr float PATH_SAFETY_CHECK_INTERVAL = 1.0f;
	constexpr float PATH_SAFETY_RAY_HEIGHT = 1.0f;
	constexpr float PATH_SAFETY_NORMAL_MIN = 0.6f;
	constexpr float PATH_SAFETY_HEIGHT_DIFF_MAX = 0.8f;
	constexpr float PATH_SAFETY_TOTAL_HEIGHT_DIFF = 1.0f;
	constexpr float FALL_DEATH_Y = -10.0f;
}

Enemy::Enemy(VECTOR pos,const CharacterStatus& status, Player* _target, ENEMYTYPE type)
	: Character(pos, status)
	, target(_target)
	, currentNodeID(0)
	, isDirectPathSafe(false)
	, type(type)
	, nowSpawned(true)
	, spawnedTimer(SPAWN_INVINCIBLE_TIME){ 
	pathUpdateTimer = (GetRand(PATH_UPDATE_RANDOM) / 100.0f);
}

void Enemy::OnHit(int damage,WeaponID id) {
	if (!alive || nowSpawned) return;
	TakeDamage(damage,id);
}

VECTOR Enemy::GetNextNodeID()const {
	if (currentPath.empty() || currentNodeID >= currentPath.size()) {
		return VGet(0.0f, 0.0f, 0.0f);
	}
	return currentPath[currentNodeID];
}

bool Enemy::CheckLineSight(const Character* target,float height)const {
	VECTOR myEye = VAdd(position, VGet(0, currentEyeHeight, 0)); 
	VECTOR targetEye = VAdd(target->GetPos(), VGet(0, height, 0));
	MV1_COLL_RESULT_POLY hitResult = MV1CollCheck_Line(stageHandle, -1, myEye, targetEye);
	return hitResult.HitFlag == 0; //ヒットしていないなら見えている
}

bool Enemy::CheckPathSafety(VECTOR targetPos)const {
	VECTOR toTarget = VSub(targetPos, position);
	float distXZ = VSize(VGet(toTarget.x, 0, toTarget.z));
	int sample = static_cast<int>(distXZ / 1.0f);
	float prevY = position.y;

	for (int i = 1; i <= sample; i++) {
		float t = (float)i / sample;
		VECTOR checkPos = VAdd(position, VScale(toTarget, t));

		VECTOR s = VGet(checkPos.x, prevY + PATH_SAFETY_RAY_HEIGHT, checkPos.z);
		VECTOR e = VGet(checkPos.x, prevY - PATH_SAFETY_RAY_HEIGHT, checkPos.z);
		MV1_COLL_RESULT_POLY ground = MV1CollCheck_Line(stageHandle, -1, s, e);
		if (ground.HitFlag == 0 || ground.Normal.y < PATH_SAFETY_NORMAL_MIN || std::abs(ground.HitPosition.y - prevY)> PATH_SAFETY_HEIGHT_DIFF_MAX) {
			return false;
		}
		prevY = ground.HitPosition.y;
	}
	float heightDiff = targetPos.y - position.y;
	if (std::abs(heightDiff) > PATH_SAFETY_TOTAL_HEIGHT_DIFF) {
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
		pathUpdateTimer = 0.5f + (GetRand(PATH_UPDATE_RANDOM) / 100.0f);
	}

	VECTOR moveTarget = target->GetPos();
	if (!isDirectPathSafe && HasPath()) {
		moveTarget = GetNextNodeID();
		VECTOR toNode = VSub(moveTarget, position);
		toNode.y = 0.0f;
		if (VSize(toNode) < PATH_NODE_REACHED_DIST) {
			AdvancePathIndex();
			if (HasPath()) {
				moveTarget = GetNextNodeID();
			}
		}
	}
	return moveTarget;
}

bool Enemy::CheckFall() const{ return position.y < FALL_DEATH_Y; }