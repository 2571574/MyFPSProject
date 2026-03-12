#include "RollingEnemy.h"
#include "Player.h"
#include "EnemyManager.h"

RollingEnemy::RollingEnemy(VECTOR pos, Player* target)
	:Enemy(pos, CHARA_STATUS::ROLL_ENEMY, target,ENEMYTYPE::ROLLING)
	, isExploding(false)
	, explodeTimer(2.0f)
	, triggerDist(5.0f) {
	explodeSpec = ENEMY_GUN::DESTRUCT;
}

void RollingEnemy::Update() {
	float dt = Time::GetIns().GetDelta();
	if (nowSpawned) {
		spawnedTimer -= dt;
		if (spawnedTimer <= 0.0f) {
			nowSpawned = false;
		}
		return;
	}
	if (hp <= 0) {
		isExploding = true;
		explodeTimer = 2.0f;
	}

	float distToPlayer = VSize(VSub(target->GetPos(), position));

	if (!isExploding) {
		VECTOR moveTarget = UpdateNavigation(target, dt);
		VECTOR dir = VNorm(VSub(moveTarget, position));
		dir.y = 0.0f;

		UpdateVelocity(dir);
		UpdatePhysics();

		if (distToPlayer < triggerDist) {
			isExploding = true;
		}
	}
	else {
		ApplyMovement(VGet(0, 0, 0), stageHandle);
		explodeTimer -= dt;

		if (explodeTimer <= 0.0f) {
			Action();
		}
	}
}

void RollingEnemy::Action() {
	CollisionManager::GetIns().ProcessExplotion(position, explodeSpec.explodeArea, explodeSpec.damage, status.teamID);

	alive = false;
}

void RollingEnemy::Draw() {
	int color = GetColor(255, 165, 0);
	if (isExploding) {
		if ((int)(explodeTimer * 10) % 2 == 0)color = GetColor(255, 0, 0);
	}
	float bodyRad = status.width / 2.0f;
	VECTOR bottom = VAdd(position, VGet(0.0f, bodyRad, 0.0f));
	VECTOR top = VAdd(position, VGet(0, currentHeight - bodyRad, 0));
	DrawCapsule3D(bottom, top, bodyRad, 16, color, color, true);

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

void RollingEnemy::UpdatePhysics() {
	float dt = Time::GetIns().GetDelta();
	float dt60 = dt * 60.0f;
	float radius = status.width / 2.0f;

	velocity.y += -0.008f * dt60;

	VECTOR nextPos = VAdd(position, VScale(velocity, dt60));

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

	VECTOR sphereCenter = VAdd(nextPos, VGet(0, radius, 0));
	MV1_COLL_RESULT_POLY_DIM wallHitDim = MV1CollCheck_Sphere(stageHandle, -1, sphereCenter, radius);
	
	//ぶつかっているポリゴンの数だけループ
	for (int i = 0; i < wallHitDim.HitNum; i++) {
		//当たっているポリゴンの法線ベクトルを取得
		VECTOR normal = wallHitDim.Dim[i].Normal;

		if (normal.y >= 0.4f) continue;
		//カプセルの下を基準
		VECTOR checkPos = nextPos;
		// 法線ベクトルが下を向いているときのみ上を基準に計算
		if (normal.y < -0.1f)checkPos = VAdd(nextPos, VGet(0, currentHeight - radius, 0));

		//カプセルの中心から壁の面の最短距離
		float distance = VDot(VSub(checkPos, wallHitDim.Dim[i].Position[0]), normal);
		//距離が半径より短い時
		if (distance < radius && distance >= 0.0f) {
			//めり込んだ分だけ押し出す
			float pushOver = radius - distance;

			VECTOR pushVec = VScale(normal, pushOver);

			pushVec.y = 0.0f;

			nextPos = VAdd(nextPos, pushVec);
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


VECTOR RollingEnemy::UpdateNavigation(const Character* target, float dt) {
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
		float distSq = VSquareSize(toNode);

		bool reached = false;
		if (distSq < 1.0f * 1.0f) {
			reached = true;
		}
		else {
			if (VSquareSize(velocity) > 0.0f) {
				if (VDot(velocity, toNode) < 0.0f) {
					reached = true;
				}
			}
		}
	
		if (reached) {
			AdvancePathIndex();
			if (HasPath())moveTarget = GetNextNodeID();
		}
	}
	return moveTarget;
}