#include "Param/Global.h"
#include "Param/Chara.h"
#include "Param/System.h"
#include "Player.h"
#include "RollingEnemy.h"
#include "EnemyManager.h"
#include "EffectManager.h"
#include "SoundManager.h"


RollingEnemy::RollingEnemy(VECTOR pos, Player* target)
	:Enemy(pos, CHARA_STATUS::ROLL_ENEMY, target, ENEMYTYPE::ROLLING)
	, isExploding(false)
	, explodeTimer(Chara::Rolling::EXPLODE_TIME)
	, triggerDist(Chara::Rolling::TRIGGER_DISTANCE)
	, alerttimer(0.0f)
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

	//HPが0以下になったら爆発開始
	if (hp <= 0 && !isExploding) {
		isExploding = true;
		alerttimer = 0.0f;
		explodeTimer = Chara::Rolling::EXPLODE_TIME;
		alertDuration = SoundManager::GetIns().GetSoundDuration("Resource/Sound/alert.wav");
		if (alertDuration <= 0.0f) alertDuration = Chara::Rolling::ALERT_DURATION_FALLBACK;
	}

	float distToPlayer = VSize(VSub(target->GetPos(), position));

	//爆発していないときはプレイヤーに向かって移動
	if (!isExploding) {
		VECTOR moveTarget = UpdateNavigation(target, dt);
		VECTOR dir = VNorm(VSub(moveTarget, position));
		dir.y = 0.0f;

		UpdateVelocity(dir);
		UpdatePhysics();

		if (distToPlayer < triggerDist) {
			isExploding = true;
			alerttimer = 0.0f;
			alertDuration = SoundManager::GetIns().GetSoundDuration("Resource/Sound/alert.wav");
			if (alertDuration <= 0.0f) alertDuration = Chara::Rolling::ALERT_DURATION_FALLBACK;
		}
	}

	//爆発しているときは移動を停止
	else {
		ApplyMovement(VGet(0.0f, 0.0f, 0.0f), stageHandle);
		explodeTimer -= dt;
		alerttimer -= dt;

		if (alerttimer <= 0.0f && explodeTimer > 0.0f) {
			SoundManager::GetIns().Play3DSE("Resource/Sound/alert.wav", position, Chara::Rolling::ALERT_SOUND_RADIUS);
			alerttimer = alertDuration;
		}

		if (explodeTimer <= 0.0f) {
			Action();
		}
	}
}

//爆発処理
void RollingEnemy::Action() {
	SoundManager::GetIns().StopSE("Resource/Sound/alert.wav");
	CollisionManager::GetIns().ProcessExplosion(position, explodeSpec.explodeArea, explodeSpec.damage, explodeSpec.knockbackP, true, status.teamID, explodeSpec.id, explodeSpec.friendlyFire);
	EffectManager::GetIns().CreateExplosionEffect(position, explodeSpec.explodeArea, GetColor(Chara::Rolling::EXPLOSION_EFFECT_COLOR.r, Chara::Rolling::EXPLOSION_EFFECT_COLOR.g, Chara::Rolling::EXPLOSION_EFFECT_COLOR.b));
	alive = false;
}

void RollingEnemy::Draw() {
	int color = GetColor(Chara::Rolling::COLOR_NORMAL.r, Chara::Rolling::COLOR_NORMAL.g, Chara::Rolling::COLOR_NORMAL.b);

	if (onHitFlashTimer > 0.0f) color = GetColor(Global::Palette::WHITE.r, Global::Palette::WHITE.g, Global::Palette::WHITE.b);

	if (isExploding) {
		if (static_cast<int>(explodeTimer * Chara::Rolling::EXPLOSION_FLASH_TIME_SCALE) % Chara::Rolling::EXPLOSION_FLASH_MODULO == 0) color = GetColor(Chara::Rolling::COLOR_EXPLODE.r, Chara::Rolling::COLOR_EXPLODE.g, Chara::Rolling::COLOR_EXPLODE.b);
	}

	float bodyRad = status.width / 2.0f;
	VECTOR bottom = VAdd(position, VGet(0.0f, bodyRad, 0.0f));
	VECTOR top = VAdd(position, VGet(0.0f, currentHeight - bodyRad, 0.0f));
	int fillFlag = nowSpawned ? FALSE : TRUE;

	//影
	if (!nowSpawned) {
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, Chara::Base::SHADOW_ALPHA);
		VECTOR shadowPos1 = VAdd(position, VGet(0.0f, Chara::Base::SHADOW_OFFSET_Y_HIGH, 0.0f));
		VECTOR shadowPos2 = VAdd(position, VGet(0.0f, Chara::Base::SHADOW_OFFSET_Y_LOW, 0.0f));
		DrawCone3D(shadowPos1, shadowPos2, status.width / Chara::Base::SHADOW_WIDTH_DIVISOR, System::Window::CIRCLE_DIVNUM, GetColor(Global::Palette::BLACK.r, Global::Palette::BLACK.g, Global::Palette::BLACK.b), GetColor(Global::Palette::BLACK.r, Global::Palette::BLACK.g, Global::Palette::BLACK.b), TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}

	//本体
	DrawCapsule3D(bottom, top, bodyRad, System::Window::CIRCLE_DIVNUM, color, color, fillFlag);

	if (isExploding) {
		DrawSphere3D(position, explodeSpec.explodeArea, System::Window::CIRCLE_DIVNUM, color, color, false);
	}
}

void RollingEnemy::UpdatePhysics() {
	float dt = Time::GetIns().GetDelta();
	float dt60 = dt * Global::Math::FPS_BASE;
	float radius = status.width / 2.0f;

	velocity.y += Chara::Base::GRAVITY * dt60;

	VECTOR nextPos = VAdd(position, VScale(velocity, dt60));

	//地面との当たり判定
	if (velocity.y <= 0.0f) {
		float offset = radius * Chara::Base::CAP_SIDE_OFFSET;

		VECTOR rayOffsets[Chara::Base::RAY_COUNT] = {
			VGet(0.0f, 0.0f, 0.0f),
			VGet(offset, 0.0f, 0.0f),
			VGet(-offset, 0.0f, 0.0f),
			VGet(0.0f, 0.0f, offset),
			VGet(0.0f, 0.0f, -offset),
		};

		bool hitGroundThisFrame = false;
		float highestY = Chara::Rolling::HIGHEST_Y_SENTINEL;

		for (int i = 0; i < Chara::Base::RAY_COUNT; i++) {
			VECTOR basePos = VAdd(nextPos, rayOffsets[i]);
			VECTOR start = VAdd(basePos, VGet(0.0f, radius + Chara::Base::STEP_RAY_START, 0.0f));
			VECTOR end = VAdd(basePos, VGet(0.0f, Chara::Base::STEP_RAY_END, 0.0f));
			MV1_COLL_RESULT_POLY groundHit = MV1CollCheck_Line(stageHandle, -1, start, end);

			if (groundHit.HitFlag == TRUE && groundHit.Normal.y > Chara::Base::GROUND_NORMAL_MIN) {
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

	//壁との当たり判定
	VECTOR sphereCenter = VAdd(nextPos, VGet(0.0f, radius, 0.0f));
	MV1_COLL_RESULT_POLY_DIM wallHitDim = MV1CollCheck_Sphere(stageHandle, -1, sphereCenter, radius);

	for (int i = 0; i < wallHitDim.HitNum; i++) {
		VECTOR normal = wallHitDim.Dim[i].Normal;

		if (normal.y > Chara::Base::GROUND_NORMAL_MIN) continue;

		VECTOR checkPos = nextPos;

		if (normal.y < Chara::Base::CEILING_NORMAL_MAX) {
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
		pathUpdateTimer = Chara::EnemyCommon::PATH_UPDATE_BASE_INTERVAL + (GetRand(Chara::EnemyCommon::PATH_UPDATE_RANDOM) / Global::Math::PERCENT_MAX);
	}

	VECTOR moveTarget = target->GetPos();

	if (!isDirectPathSafe && HasPath()) {
		moveTarget = GetNextNodeID();
		VECTOR toNode = VSub(moveTarget, position);
		toNode.y = 0.0f;
		float distSq = VSquareSize(toNode);

		bool reached = false;
		if (distSq < (Chara::EnemyCommon::PATH_NODE_REACHED_DIST * Chara::EnemyCommon::PATH_NODE_REACHED_DIST)) {
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