#include "Character.h"
#include "CollisionManager.h"
#include "Time.h"
#include <cmath>

/// <summary>
/// コンストラクタ　位置とステータスを受け取る
/// </summary>
/// <param name="_position">初期座標</param>
/// <param name="_status">キャラのステータス</param>
Character::Character(VECTOR _position, CharacterStatus& _status) :
	position(_position)
	, velocity({ 0,0,0 })
	, knockback({ 0,0,0 })
	, status(_status)
	, hp(_status.maxHP)
	, alive(true)
	, crouch(false)
	, currentHeight(_status.height)
	, currentEyeHeight(_status.eyeHeight)
	, modelHandle(-1) {

	CollisionManager::GetIns().Register(this);
};		//コンストラクタ

Character::~Character() {
	CollisionManager::GetIns().Unregister(this);
	if (modelHandle != -1) MV1DeleteModel(modelHandle);
}

void Character::ApplyMovement(VECTOR moveDir,int stageHandle) {
	float dt = Time::GetIns().GetDelta();
	UpdateVelocity(moveDir);
	UpdatePhysics(stageHandle);
}
void Character::UpdateVelocity(VECTOR moveDir) {
	float dt = Time::GetIns().GetDelta();
	float dt60 = dt * 60.0f;
	if (VSize(moveDir) > 0.0f) {
		velocity.x += moveDir.x * status.accel * dt60;
		velocity.z += moveDir.z * status.accel * dt60;
	}

	float finalFriction = std::pow(status.friction, dt60);
	velocity.x *= finalFriction;
	velocity.z *= finalFriction;
}


void Character::UpdatePhysics(int stageHandle) {
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
