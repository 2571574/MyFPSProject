#include "Enemy.h"
#include <cmath>
#include "EnemyManager.h"
void Enemy::OnHit(int damage) {
	TakeDamage(damage);
	Debug::Log("HIT");
}

VECTOR Enemy::GetNextNodeID()const {
	if (currentPath.empty() || currentNodeID >= currentPath.size()) {
		return VGet(0.0f, 0.0f, 0.0f);
	}
	return currentPath[currentNodeID];
}

void Enemy::ApplyMovement(VECTOR moveDir, float dt,float velRatio) {
	UpdateVelocity(moveDir, dt,velRatio);
	UpdatePhysics(dt);
}
void Enemy::UpdateVelocity(VECTOR moveDir, float dt,float velRatio) {
	float dt60 = dt * 60.0f;
	if (VSize(moveDir) > 0.0f) {
		float accel = status.accel * velRatio;
		velocity.x += moveDir.x * accel * dt60;
		velocity.z += moveDir.z * accel * dt60;
	}

	float finalFriction = std::pow(status.friction, dt60);
	velocity.x *= finalFriction;
	velocity.z *= finalFriction;
}


void Enemy::UpdatePhysics(float dt) {
	float dt60 = dt * 60.0f;
	float radius = status.width;

	velocity.y += -0.008f * dt60;
	
	VECTOR nextPos = VAdd(position,VScale(velocity,dt60));

	if (velocity.y <= 0.0f) {
		float offset = radius * 0.8f;

		VECTOR rayOffsets[5] = {
			VGet(0.0f,0.0f,0.0f),
			VGet(offset,0.0f,0.0f),
			VGet(-offset,0.0f,0.0f),
			VGet(0.0f,0.0f,offset),
			VGet(0.0f,0.0f,-offset),
		};
		bool hitGroundThisFrame = false;
		float highestY = -999;
		for (int i = 0; i < 5; i++) {
			VECTOR basePos = VAdd(nextPos, rayOffsets[i]);
			//足元からレイを打つ
			VECTOR start = VAdd(basePos, VGet(0, radius + 0.1f, 0));
			VECTOR end = VAdd(basePos, VGet(0, -0.2f, 0));
			//床と当たったか判定する
			MV1_COLL_RESULT_POLY groundHit = MV1CollCheck_Line(stageHandle, -1, start, end);

			//当たっていた時、その面の法線が上を向いていたら地面とみなす
			if (groundHit.HitFlag == 1 && groundHit.Normal.y > 0.3f) {
				if (groundHit.HitPosition.y > highestY) {
					highestY = groundHit.HitPosition.y;
					hitGroundThisFrame = true;
				}
			}
		}
		if (hitGroundThisFrame) {
			//地面の高さにして、落下をなくす
			nextPos.y = highestY;
			velocity.y = 0;
		}
	}

	VECTOR capBottom = VAdd(nextPos, VGet(0, radius + 0.3f, 0));
	VECTOR capTop = VAdd(nextPos, VGet(0, currentHeight - radius, 0));
	MV1_COLL_RESULT_POLY_DIM wallHitDim = MV1CollCheck_Capsule(stageHandle, -1, capBottom, capTop, radius);
	VECTOR totalPush = VGet(0, 0, 0);
	int hitCount = 0;
	//ぶつかっているポリゴンの数だけループ
	for (int i = 0; i < wallHitDim.HitNum; i++) {
		//当たっているポリゴンの法線ベクトルを取得
		VECTOR normal = wallHitDim.Dim[i].Normal;

		if (normal.y >= 0.4f) continue;
		//カプセルの下を基準
		VECTOR checkPos = nextPos;
		// 法線ベクトルが下を向いているときのみ上を基準に計算
		if (normal.y < -0.1f)checkPos = VAdd(nextPos, VGet(0,currentHeight - radius, 0));

		//カプセルの中心から壁の面の最短距離
		float distance = VDot(VSub(checkPos, wallHitDim.Dim[i].Position[0]), normal);
		//距離が半径より短い時
		if (distance < radius && distance >= 0.0f) {
			//めり込んだ分だけ押し出す
			float pushOver = radius - distance;

			VECTOR pushVec = VScale(normal, pushOver);

			pushVec.y = 0.0f;

			nextPos = VAdd(nextPos, pushVec);
			hitCount++;
			float dotVec = velocity.x * normal.x + velocity.z * normal.z;
			if (dotVec < 0.0f) {
				velocity.x -= normal.x * dotVec;
				velocity.z -= normal.z * dotVec;
			}


		}
	}
	//当たり判定で使ったメモリを解放
	DxLib::MV1CollResultPolyDimTerminate(wallHitDim);

	position = nextPos;
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