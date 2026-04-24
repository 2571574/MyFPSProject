#include "Character.h"
#include "CollisionManager.h"
#include "Time.h"
#include "Param/Global.h"
#include "Param/Chara.h"
#include "Param/System.h"

#include <cmath>
#include <cfloat>

Character::Character(VECTOR _position, const CharacterStatus& _status) :
	position(_position)
	, velocity({ 0,0,0 })
	, knockback({ 0,0,0 })
	, status(_status)
	, hp(_status.maxHP)
	, alive(true)
	, crouch(false)
	, onGround(true)
	, currentHeight(_status.height)
	, currentEyeHeight(_status.eyeHeight) {

	CollisionManager::GetIns().Register(this);
};

Character::~Character() {
	CollisionManager::GetIns().Unregister(this);
}

void Character::ApplyMovement(VECTOR moveDir, int stageHandle) {
	UpdateVelocity(moveDir);
	UpdatePhysics(stageHandle);
}

void Character::UpdateVelocity(VECTOR moveDir) {
	float dt = Time::GetIns().GetDelta();
	float dt60 = dt * Global::Math::FPS_BASE;
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
	float dt60 = dt * Global::Math::FPS_BASE;
	float radius = status.width / 2.0f;

	float currentkbFriction = onGround ? Chara::Base::GROUND_KB_FRICTION : Chara::Base::AIR_KB_FRICTION;
	float kbFriction = std::pow(currentkbFriction, dt60);
	knockback.x *= kbFriction;
	knockback.z *= kbFriction;

	if (VSize(knockback) < Chara::Base::MOVEMENT_MIN) {
		knockback = VGet(0.0f, 0.0f, 0.0f);
	}

	velocity.y += Chara::Base::GRAVITY * dt60;

	VECTOR totalVelocity = VAdd(velocity, knockback);
	VECTOR totalMove = VScale(totalVelocity, dt60);
	float moveDist = VSize(totalMove);

	float maxStep = radius * 0.5f;
	int stepCount = 1;
	if (moveDist > maxStep) {
		stepCount = (int)std::ceil(moveDist / maxStep);
	}

	VECTOR stepMove = VScale(totalMove, 1.0f / stepCount);
	VECTOR currentPos = position;

	bool isGroundedThisFrame = false;

	for (int step = 0; step < stepCount; ++step) {
		VECTOR nextPos = VAdd(currentPos, stepMove);
		if (totalVelocity.y <= 0.0f) {
			float offset = radius * Chara::Base::CAP_SIDE_OFFSET;

			VECTOR rayOffsets[Chara::Base::RAY_COUNT] = {
				VGet(0.0f,0.0f,0.0f),
				VGet(offset,0.0f,0.0f),
				VGet(-offset,0.0f,0.0f),
				VGet(0.0f,0.0f,offset),
				VGet(0.0f,0.0f,-offset),
			};
			bool hitGroundThisFrame = false;
			float highestY = -FLT_MAX;

			for (int i = 0; i < Chara::Base::RAY_COUNT; i++) {
				VECTOR basePos = VAdd(nextPos, rayOffsets[i]);
				//足元からレイを打つ
				VECTOR start = VAdd(basePos, VGet(0, radius + Chara::Base::STEP_RAY_START, 0));
				VECTOR end = VAdd(basePos, VGet(0, Chara::Base::STEP_RAY_END, 0));
				//床と当たったか判定する
				MV1_COLL_RESULT_POLY groundHit = MV1CollCheck_Line(stageHandle, -1, start, end);

				//当たっていた時、その面の法線が上を向いていたら地面とみなす
				if (groundHit.HitFlag == TRUE && groundHit.Normal.y > Chara::Base::GROUND_NORMAL_MIN) {
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
				totalVelocity.y = 0;
				stepMove.y = 0;
				isGroundedThisFrame = true;
			}
		}

		VECTOR capBottom = VAdd(nextPos, VGet(0, radius + Chara::Base::CAP_BOTTOM_OFFSET, 0));
		VECTOR capTop = VAdd(nextPos, VGet(0, currentHeight - radius, 0));
		MV1_COLL_RESULT_POLY_DIM wallHitDim = MV1CollCheck_Capsule(stageHandle, -1, capBottom, capTop, radius);
		VECTOR totalPush = VGet(0, 0, 0);
		int hitCount = 0;
		//ぶつかっているポリゴンの数だけループ
		for (int i = 0; i < wallHitDim.HitNum; i++) {
			//当たっているポリゴンの法線ベクトルを取得
			VECTOR normal = wallHitDim.Dim[i].Normal;

			if (normal.y > Chara::Base::GROUND_NORMAL_MIN) continue;
			//カプセルの下を基準
			VECTOR checkPos = nextPos;
			// 法線ベクトルが下を向いているときのみ上を基準に計算
			if (normal.y < Chara::Base::CEILING_NORMAL_MAX)checkPos = VAdd(nextPos, VGet(0, currentHeight - radius, 0));

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

				float dotKB = knockback.x * normal.x + knockback.z * normal.z;
				if (dotKB < 0.0f) {
					knockback.x -= normal.x * dotKB;
					knockback.z -= normal.z * dotKB;
				}

				float dotStep = stepMove.x * normal.x + stepMove.z * normal.z;
				if (dotStep < 0.0f) {
					stepMove.x -= normal.x * dotStep;
					stepMove.z -= normal.z * dotStep;
				}

			}
		}
		//当たり判定で使ったメモリを解放
		DxLib::MV1CollResultPolyDimTerminate(wallHitDim);
		currentPos = nextPos;
	}
	position = currentPos;
	onGround = isGroundedThisFrame;
}

void Character::ResolveWallPenetration(int stagehandle) {
	if (stagehandle == -1) return;

	float radius = status.width / 2.0f;
	VECTOR capBottom = VAdd(position, VGet(0, radius + Chara::Base::CAP_BOTTOM_OFFSET, 0));
	VECTOR capTop = VAdd(position, VGet(0, currentHeight - radius, 0));

	MV1_COLL_RESULT_POLY_DIM wallHitDim = MV1CollCheck_Capsule(stagehandle, -1, capBottom, capTop, radius);

	for (int i = 0; i < wallHitDim.HitNum; i++) {
		VECTOR normal = wallHitDim.Dim[i].Normal;

		if (normal.y > Chara::Base::GROUND_NORMAL_MIN) continue;

		VECTOR checkPos = position;
		if (normal.y < Chara::Base::CEILING_NORMAL_MAX) checkPos = VAdd(position, VGet(0, currentHeight - radius, 0));

		float distance = VDot(VSub(checkPos, wallHitDim.Dim[i].Position[0]), normal);

		if (distance < radius && distance >= 0.0f) {
			float pushOver = radius - distance;
			VECTOR pushVec = VScale(normal, pushOver);
			pushVec.y = 0.0f;
			position = VAdd(position, pushVec);
		}
	}
	DxLib::MV1CollResultPolyDimTerminate(wallHitDim);
}