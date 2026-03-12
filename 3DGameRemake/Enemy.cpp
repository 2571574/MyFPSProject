#include "Enemy.h"
#include <cmath>
#include "EnemyManager.h"
Enemy::Enemy(VECTOR pos, CharacterStatus& status, Player* _target, ENEMYTYPE type)
	: Character(pos, status)
	, target(_target)
	, currentNodeID(0)
	, isDirectPathSafe(false)
	, type(type)
	, nowSpawned(true)
	, spawnedTimer(1.5f){ 
	pathUpdateTimer = (GetRand(50) / 100.0f);
}

void Enemy::OnHit(int damage) {
	if (!alive || nowSpawned) return;
	TakeDamage(damage);
}

VECTOR Enemy::GetNextNodeID()const {
	if (currentPath.empty() || currentNodeID >= currentPath.size()) {
		return VGet(0.0f, 0.0f, 0.0f);
	}
	return currentPath[currentNodeID];
}

bool Enemy::CheckLineSight(const Character* target,float height) {
	VECTOR myEye = VAdd(position, VGet(0, currentEyeHeight, 0));
	VECTOR targetEye = VAdd(target->GetPos(), VGet(0, height, 0));
	MV1_COLL_RESULT_POLY hitResult = MV1CollCheck_Line(stageHandle, -1, myEye, targetEye);
	return hitResult.HitFlag == 0; //ヒットしていないなら見えている
}

bool Enemy::CheckPathSafety(VECTOR targetPos) {
	VECTOR toTarget = VSub(targetPos, position);
	float distXZ = VSize(VGet(toTarget.x, 0, toTarget.z));
	int sample = (int)(distXZ / 1.0f);
	float prevY = position.y;

	for (int i = 1; i <= sample; i++) {
		float t = (float)i / sample;
		VECTOR checkPos = VAdd(position, VScale(toTarget, t));

		VECTOR s = VGet(checkPos.x, prevY + 1.0f, checkPos.z);
		VECTOR e = VGet(checkPos.x, prevY - 1.0f, checkPos.z);
		MV1_COLL_RESULT_POLY ground = MV1CollCheck_Line(stageHandle, -1, s, e);
		if (ground.HitFlag == 0 || ground.Normal.y < 0.6f || std::abs(ground.HitPosition.y - prevY)> 0.8f) {
			return false;
		}
		prevY = ground.HitPosition.y;
	}
	float heightDiff = targetPos.y - position.y;
	if (std::abs(heightDiff) > 1.0f) {
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
		pathUpdateTimer = 0.5f + (GetRand(50) / 100.0f);
	}

	VECTOR moveTarget = target->GetPos();
	if (!isDirectPathSafe && HasPath()) {
		moveTarget = GetNextNodeID();
		VECTOR toNode = VSub(moveTarget, position);
		toNode.y = 0.0f;
		if (VSize(toNode) < 1.0f) {
			AdvancePathIndex();
		}
	}
	return moveTarget;
}