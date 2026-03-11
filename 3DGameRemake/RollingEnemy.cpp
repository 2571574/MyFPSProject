#include "RollingEnemy.h"
#include "Player.h"
#include "EnemyManager.h"

RollingEnemy::RollingEnemy(VECTOR pos, Player* target)
	:Enemy(pos, CHARA_STATUS::ROLL_ENEMY, target)
	, isExploding(false)
	, explodeTimer(2.0f)
	, triggerDist(5.0f) {
	explodeSpec = ENEMY_GUN::DESTRUCT;
}

void RollingEnemy::Update() {

	float dt = Time::GetIns().GetDelta();
	float distToPlayer = VSize(VSub(target->GetPos(), position));

	if (!isExploding) {
		VECTOR moveTarget = UpdateNavigation(target, dt);
		VECTOR dir = VNorm(VSub(moveTarget, position));
		dir.y = 0.0f;

		UpdateVelocity(dir, dt);
		UpdatePhysics(dt);

		if (distToPlayer < triggerDist) {
			isExploding = true;
		}
	}
	else {
		ApplyMovement(VGet(0, 0, 0), dt);
		explodeTimer -= dt;

		if (explodeTimer <= 0.0f) {
			Action();
		}
	}
}

void RollingEnemy::Action() {
	float radius = explodeSpec.explodeArea;

	float distToPlayer = VSize(VSub(target->GetPos(), position));

	if (distToPlayer <= radius) {
		float damageRate = 1.0f - (distToPlayer / radius);
		target->TakeDamage((int)(explodeSpec.damage) * damageRate);
	}

	auto& enemies = EnemyManager::GetIns().GetEnemies();

	for(auto&enemy : enemies){
		if (!enemy || !enemy->IsAlive() || enemy.get() == this) continue;

		float dist = VSize(VSub(enemy->GetPos(), position));
		if (dist <= radius) {
			enemy->OnHit(explodeSpec.damage);
		}
	}

	alive = false;
}

void RollingEnemy::Draw() {
	int color = GetColor(255, 165, 0);
	if (isExploding) {
		if ((int)(explodeTimer * 10) % 2 == 0)color = GetColor(255, 0, 0);
	}

	DrawSphere3D(VAdd(position, VGet(0.0f, status.height, 0.0f)), status.width, 16, color, color, TRUE);
}

void RollingEnemy::OnHit(int damage) {
	hp -= damage;
	if (hp <= 0) {
		isExploding = true;
		explodeTimer = 2.0f;
	}
	Debug::Log("RollingEnemyHit");
}

void RollingEnemy::UpdatePhysics(float dt) {
	float dt60 = dt * 60.0f;
	float radius = status.width;

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