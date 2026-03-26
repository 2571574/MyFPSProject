#include "RollingEnemy.h"
#include "Player.h"
#include "EnemyManager.h"
#include "EffectManager.h"
#include "SoundManager.h"

namespace {
	constexpr float EXPLODE_TIME = 2.0f;
	constexpr float TRIGGER_DISTANCE = 5.0f;

	constexpr float GRAVITY = -0.008f;
	constexpr float STEP_RAY_SIDE_OFFSET = 0.8f;
	constexpr float STEP_RAY_START_OFFSET = 0.1f;
	constexpr float STEP_RAY_END_OFFSET = -0.2f;
	constexpr float GROUND_NORMAL_MIN = 0.3f;
	constexpr float WALL_NORMAL_MAX = 0.4f;
	constexpr float CEILING_NORMAL_MAX = -0.1f;

	constexpr float PATH_UPDATE_BASE_TIME = 0.5f;
	constexpr int PATH_UPDATE_RANDOM_RANGE = 50;
	constexpr float PATH_NODE_REACHED_DIST_SQ = 1.0f * 1.0f;
}


RollingEnemy::RollingEnemy(VECTOR pos, Player* target)
	:Enemy(pos, CHARA_STATUS::ROLL_ENEMY, target,ENEMYTYPE::ROLLING)
	, isExploding(false)
	, explodeTimer(EXPLODE_TIME)
	, triggerDist(TRIGGER_DISTANCE)
	, beepTimer(0.0f)
	, alertDuration(0.0f) {
	explodeSpec = ENEMY_GUN::DESTRUCT;
}

void RollingEnemy::Update() {
	float dt = Time::GetIns().GetDelta();
	if (onHitFlashTimer > 0.0f) onHitFlashTimer -= dt;
	if (nowSpawned) {
		spawnedTimer -= dt;
		if (spawnedTimer <= 0.0f) {
			nowSpawned = false;
		}
		return;
	}
	if (hp <= 0 && !isExploding) {
		isExploding = true;
		beepTimer = 0.0f;
		explodeTimer = EXPLODE_TIME;
		alertDuration = SoundManager::GetIns().GetSoundDuration("Resource/Sound/alert.wav");
		if (alertDuration <= 0.0f) alertDuration = 1.0f;
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
			beepTimer = 0.0f;
			alertDuration = SoundManager::GetIns().GetSoundDuration("Resource/Sound/alert.wav");
			if (alertDuration <= 0.0f) alertDuration = 1.0f;
		}
	}
	else {
		ApplyMovement(VGet(0, 0, 0), stageHandle);
		explodeTimer -= dt;
		beepTimer -= dt;

		if (beepTimer <= 0.0f && explodeTimer > 0.0f) {
			SoundManager::GetIns().Play3DSE("Resource/Sound/alert.wav", position, 30.0f);

			beepTimer = alertDuration;
		}
		if (explodeTimer <= 0.0f) {
			Action();
		}
	}
}

void RollingEnemy::Action() {
	SoundManager::GetIns().StopSE("Resource/Sound/alert.wav");
	CollisionManager::GetIns().ProcessExplosion(position, explodeSpec.explodeArea, explodeSpec.damage,explodeSpec.knockbackP,true, status.teamID,explodeSpec.id,explodeSpec.friendlyFire);
	EffectManager::GetIns().CreateExplosionEffect(position, explodeSpec.explodeArea, GetColor(255, 100, 0));
	alive = false;
}

void RollingEnemy::Draw() {
	int color = GetColor(255, 165, 0);
	if (onHitFlashTimer > 0.0f) color = GetColor(255, 255, 255);
	if (isExploding) {
		if (static_cast<int>(explodeTimer * 10) % 2 == 0)color = GetColor(255, 0, 0);
	}
	float bodyRad = status.width / 2.0f;
	VECTOR bottom = VAdd(position, VGet(0.0f, bodyRad, 0.0f));
	VECTOR top = VAdd(position, VGet(0, currentHeight - bodyRad, 0));
	int fillFlag = nowSpawned ? FALSE : TRUE;

	if (!nowSpawned) {
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 100);
		VECTOR shadowPos1 = VAdd(position, VGet(0.0f, 0.02f, 0.0f));
		VECTOR shadowPos2 = VAdd(position, VGet(0.0f, 0.01f, 0.0f));
		DrawCone3D(shadowPos1, shadowPos2, status.width / 1.5f, 16, GetColor(0, 0, 0), GetColor(0, 0, 0), TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}

	DrawCapsule3D(bottom, top, bodyRad, CIRCLE_DIVNUM, color, color, fillFlag);

	if (isExploding) {
		DrawSphere3D(position, explodeSpec.explodeArea, CIRCLE_DIVNUM, color, color, false);
	}
}

void RollingEnemy::UpdatePhysics() {
	float dt = Time::GetIns().GetDelta();
	float dt60 = dt * 60.0f;
	float radius = status.width / 2.0f;

	velocity.y += GRAVITY * dt60;

	VECTOR nextPos = VAdd(position, VScale(velocity, dt60));

	if (velocity.y <= 0.0f) {
		float offset = radius * STEP_RAY_SIDE_OFFSET;

		VECTOR rayOffsets[5] = {
			VGet(0.0f, 0.0f, 0.0f),
			VGet(offset, 0.0f, 0.0f),
			VGet(-offset, 0.0f, 0.0f),
			VGet(0.0f, 0.0f, offset),
			VGet(0.0f, 0.0f, -offset),
		};
		bool hitGroundThisFrame = false;
		float highestY = -999.0f;

		for (int i = 0; i < 5; i++) {
			VECTOR basePos = VAdd(nextPos, rayOffsets[i]);
			VECTOR start = VAdd(basePos, VGet(0.0f, radius + STEP_RAY_START_OFFSET, 0.0f));
			VECTOR end = VAdd(basePos, VGet(0.0f, STEP_RAY_END_OFFSET, 0.0f));
			MV1_COLL_RESULT_POLY groundHit = MV1CollCheck_Line(stageHandle, -1, start, end);

			if (groundHit.HitFlag == 1 && groundHit.Normal.y > GROUND_NORMAL_MIN) {
				if (groundHit.HitPosition.y > highestY) {
					highestY = groundHit.HitPosition.y;
					hitGroundThisFrame = true;
				}
			}
		}
		if (hitGroundThisFrame) {
			nextPos.y = highestY;
			velocity.y = 0.0f;
		}
	}

	VECTOR sphereCenter = VAdd(nextPos, VGet(0.0f, radius, 0.0f));
	MV1_COLL_RESULT_POLY_DIM wallHitDim = MV1CollCheck_Sphere(stageHandle, -1, sphereCenter, radius);

	for (int i = 0; i < wallHitDim.HitNum; i++) {
		VECTOR normal = wallHitDim.Dim[i].Normal;

		if (normal.y >= WALL_NORMAL_MAX) continue;

		VECTOR checkPos = nextPos;
		if (normal.y < CEILING_NORMAL_MAX) {
			checkPos = VAdd(nextPos, VGet(0.0f, currentHeight - radius, 0.0f));
		}

		float distance = VDot(VSub(checkPos, wallHitDim.Dim[i].Position[0]), normal);

		if (distance < radius && distance >= 0.0f) {
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
		pathUpdateTimer = PATH_UPDATE_BASE_TIME + (GetRand(PATH_UPDATE_RANDOM_RANGE) / 100.0f);
	}

	VECTOR moveTarget = target->GetPos();
	if (!isDirectPathSafe && HasPath()) {
		moveTarget = GetNextNodeID();
		VECTOR toNode = VSub(moveTarget, position);
		toNode.y = 0.0f;
		float distSq = VSquareSize(toNode);

		bool reached = false;
		if (distSq <PATH_NODE_REACHED_DIST_SQ) {
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