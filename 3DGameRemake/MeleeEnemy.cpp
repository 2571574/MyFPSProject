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
	pathUpdateTimer -= dt;
	if (pathUpdateTimer <= 0.0f) {

		std::vector<VECTOR>newPath = EnemyManager::GetIns().CalculatePath(position, target->GetPos());
		SetPath(newPath);
		pathUpdateTimer = 1.0f;
		Debug::Log("pathUpdate");

		if (currentPath.size() > 1) {
			if (VSize(VSub(currentPath[0], position)) < 0.5f) {
				AdvancePathIndex();
			}
		}
	}
	bool isMoving = false;
	VECTOR moveTarget = position;

	if (HasPath()) {
		moveTarget = GetNextNodeID();
		isMoving = true;

		VECTOR toTarget = VSub(moveTarget, position);
		toTarget.y = 0.0f;
		if (VSize(toTarget)<1.0f){
			AdvancePathIndex();
		}
	}
	else {
		moveTarget = target->GetPos();
		VECTOR toTarget2D = VSub(moveTarget, position);
		toTarget2D.y = 0.0f;
		if (VSize(toTarget2D) > range * 0.8f) {
			isMoving = true;
		}
	}

	VECTOR dir = VSub(moveTarget, position);
	dir.y = 0.0f;
	if (VSize(dir) > 0.0f) {
		dir = VNorm(dir);
	}

	VECTOR nextPos = position;

	if (isMoving) {
		nextPos.x += dir.x * moveSpeed * dt;
		nextPos.z += dir.z * moveSpeed * dt;
	}

	velocity.y += -0.008f * dt60;
	nextPos.y += velocity.y * dt60;

	float radius = status.width;

	if (stageHandle != -1 && velocity.y <= 0.2f) {

		VECTOR start = VAdd(nextPos, VGet(0.0f, radius + 0.5f, 0.0f));
		VECTOR end = VAdd(nextPos, VGet(0.0f, -0.5f, 0.0f));

		MV1_COLL_RESULT_POLY groundHit = MV1CollCheck_Line(stageHandle, -1, start, end);

		if (groundHit.HitFlag == 1 && groundHit.Normal.y > 0.3f) {
			nextPos.y = groundHit.HitPosition.y;
			velocity.y = 0.0f;
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
		if (normal.y < -0.1f)checkPos = VAdd(nextPos,VGet(0,status.height - radius, 0));

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
	
	VECTOR toTarget = VSub(moveTarget, position);
	float targetDistXZ = VSize(VGet(toTarget.x,0.0f,toTarget.z));
	float targetHeight = std::abs(toTarget.y);
	if (targetDistXZ <= range && targetHeight <= status.height && attackTimer <= 0.0f) {
		Action();
	}

	Debug::Watch("BOT X",position.x);
	Debug::Watch("BOT Y",position.y);
	Debug::Watch("BOT Z",position.z);
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