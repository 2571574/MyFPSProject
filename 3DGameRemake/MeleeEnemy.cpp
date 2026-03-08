#include "MeleeEnemy.h"
#include "Player.h"
#include "Time.h"
#include <cmath>
#include "Parameter.h"
#include "EnemyManager.h"
MeleeEnemy::MeleeEnemy(VECTOR pos, Player* target) :Enemy(pos, CHARA_STATUS::MELEE_ENEMY, target), attackTimer(0.0f),range(3.0f),moveSpeed(3.0f) {}

void MeleeEnemy::Update() {
	if (hp <= 0) {
		alive = false;
		return;
	}

	float dt = Time::GetIns().GetDelta();
	float dt60 = 60.0f * dt;
	if (attackTimer > 0.0f) {
		attackTimer -= dt;
	}

	if (target == nullptr) return;

	VECTOR eyePos = VAdd(position, VGet(0.0f, status.height * 0.8f, 0.0f));
	VECTOR targetPos = target->GetPos();
	VECTOR targeteyePos = VAdd(targetPos, VGet(0.0f, target->GetStatus().height * 0.8f, 0));

	MV1_COLL_RESULT_POLY losHit = MV1CollCheck_Line(stageHandle, -1, eyePos, targeteyePos);
	bool hasLoS = (losHit.HitFlag == 0);

	bool isDirectPathSafe = false;
	if (hasLoS) {
		isDirectPathSafe = true;
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
				isDirectPathSafe = false;
				break;
			}
			prevY = ground.HitPosition.y;
		}

	}
	pathUpdateTimer -= dt;
	if (pathUpdateTimer <= 0.0f) {
		if (!isDirectPathSafe) {
			std::vector<VECTOR>newPath = EnemyManager::GetIns().CalculatePath(position, target->GetPos());
			SetPath(newPath);
			Debug::Log("pathUpdate");
		}
		pathUpdateTimer = 0.5f + (GetRand(50) / 100.0f);
	}
	VECTOR moveTarget = targetPos;
	bool usePath = !isDirectPathSafe && HasPath();

	if (usePath) {
		moveTarget = GetNextNodeID();
		VECTOR toNode = VSub(moveTarget, position);
		toNode.y = 0.0f;
		if (VSize(toNode) < 1.0f) {
			AdvancePathIndex();
		}

	}

	VECTOR dir = VSub(moveTarget,position);
	dir.y = 0.0f;
	float distToMoveTarget = VSize(dir);
	if (distToMoveTarget > 0.1f)dir = VNorm(dir);
	else dir = VGet(0.0f, 0.0f, 0.0f);


	VECTOR nextPos = position;
	if (VSize(VSub(targetPos, position)) > range * 0.8f) {
		nextPos.x += dir.x * moveSpeed * dt;
		nextPos.z += dir.z * moveSpeed * dt;
	}

	velocity.y += -0.008f * dt60;
	nextPos.y += velocity.y * dt60;

	float radius = status.width;

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
		int hitPolyIndex = -1;
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
					hitPolyIndex = groundHit.PolygonIndex;
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
	VECTOR capTop = VAdd(nextPos, VGet(0, status.height - radius, 0));
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
		if (normal.y < -0.1f)checkPos = VAdd(nextPos, VGet(0, status.height - radius, 0));

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

	float distToTarget = VSize(VSub(targetPos, position));
	if (distToTarget <= range && std::abs(targetPos.y - position.y) <= status.height && attackTimer <= 0.0f) {
		Action();
	}

	Debug::Watch("BOT X", position.x);
	Debug::Watch("BOT Y", position.y);
	Debug::Watch("BOT Z", position.z);
}



void MeleeEnemy::Draw() {
	VECTOR top = VAdd(position, VGet(0, status.height, 0));
	DrawCapsule3D(position, top, status.width, CIRCLE_DIVNUM, GetColor(255, 125, 0), GetColor(255, 125, 0), TRUE);
}

void MeleeEnemy::Action() {
	target->TakeDamage(1);

	attackTimer = 0.5f;

	Debug::Log("EnemyAttack");
}