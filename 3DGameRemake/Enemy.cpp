#include "Enemy.h"
#include "EnemyManager.h"
#include "Time.h"
#include "SoundManager.h"

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
	constexpr float STEP_LENGTH = 2.5f;
}

Enemy::Enemy(VECTOR pos,const CharacterStatus& status, Player* _target, ENEMYTYPE type)
	: Character(pos, status)
	, target(_target)
	, currentNodeID(0)
	, isDirectPathSafe(false)
	, type(type)
	, nowSpawned(true)
	, spawnedTimer(SPAWN_INVINCIBLE_TIME)
	, onHitFlashTimer(0.0f)
	, moveDistance(0.0f){ 
	pathUpdateTimer = (GetRand(PATH_UPDATE_RANDOM) / 100.0f);
}

void Enemy::OnHit(int damage,WeaponID id) {
	if (!alive || nowSpawned) return;
	TakeDamage(damage,id);
	onHitFlashTimer = 0.1f;
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

void Enemy::UpdateFootstep() {
	if (!onGround || !alive || nowSpawned) return;

	float dt = Time::GetIns().GetDelta();
	float dt60 = dt * 60.0f; // 移動距離を算出するための係数

	// XZ平面の移動速度（1フレームあたりの移動ベクトル量）
	float speed = VSize(VGet(velocity.x, 0.0f, velocity.z));

	// 僅かでも動いていれば距離を累積
	if (speed > 0.01f) {
		// 実際の移動距離を加算
		moveDistance += speed * dt60;

		// 1歩の歩幅（2.0f〜3.0fくらいで足音の頻度を調整できます。小さいほど頻繁に鳴ります）
		constexpr float STEP_LENGTH = 2.5f;

		if (moveDistance >= STEP_LENGTH) {
			// 音が聞こえやすいように radius を 40.0f に少し広げています
			SoundManager::GetIns().Play3DSE("Resource/Sound/footstep.ogg", position, 30.0f);

			// 距離をリセット（歩幅分だけ引くことで、端数を持ち越して滑らかに鳴らします）
			moveDistance -= STEP_LENGTH;
		}
	}
	else {
		// 完全に立ち止まったらリセット
		moveDistance = 0.0f;
	}
}