#include "Player.h"
#include "cmath"


Player::Player(VECTOR pos,Camera* camera)
	: Character(pos, CHARA_STATUS::PLAYER)
	, cam(camera)
	, stageHandle(-1)
	, forwardVec({ 0,0,0 })
	, rightVec({ 0,0,0 })
	, weapon(std::make_unique<Weapon>(PLAYER_GUN::SMG))
	, fov(0)
	,slidingCT(0.0f)
	, isAds(false)
	, onGround(true)
	,running(false)
	, headBob(true)
	, bobbingTimer(0)
	
{
}


Player::~Player() {
}


void Player::Update() {
	float dt = Time::GetIns().GetDelta();
	float dt60 = 60.0f * dt;
	if (slidingCT > 0.0f) {
		slidingCT -= dt;
		if (slidingCT <= 0.0f) {
			slidingCT = 0.0f;
		}
	}
	//移動入力を取得
	VECTOR moveVec = VGet(0, 0, 0);	//入力した方向ベクトル
	cam->GetForwardVec(forwardVec, rightVec);
	if (InputManager::GetIns().IsActionHold(ActionID::MOVE_FORWARD)) {
		moveVec = VAdd(moveVec, forwardVec);
	}
	if (InputManager::GetIns().IsActionHold(ActionID::MOVE_BACK)) {
		moveVec = VSub(moveVec, forwardVec);
	}
	if (InputManager::GetIns().IsActionHold(ActionID::MOVE_LEFT)) {
		moveVec = VSub(moveVec, rightVec);
	}
	if (InputManager::GetIns().IsActionHold(ActionID::MOVE_RIGHT)) {
		moveVec = VAdd(moveVec, rightVec);
	}
	//コントローラーの移動入力を取得
	int LX, LY;
	CheckKey::GetIns().GetLeftStick(LX, LY);
	float right = LX / 1000.0f;
	float forward = -LY / 1000.0f;
	moveVec = VAdd(moveVec, VScale(forwardVec, forward));
	moveVec = VAdd(moveVec, VScale(rightVec, right));
	
	//正規化
	if (VSize(moveVec) > 1.0f) {
		moveVec = VNorm(moveVec);
	}

	
	crouch = InputManager::GetIns().IsActionHold(ActionID::CROUCH);
	running = InputManager::GetIns().IsActionHold(ActionID::RUN);

	if (crouch)running = false;
	//武器の更新と入力を検知
	if (weapon) {
		if (running) {
			weapon->CancelAds();
			weapon->CancelReload();
		}
		else{
			weapon->FireInput(*this, GetCamDirection());
			weapon->AdsInput();
			weapon->ReloadInput();
		}
		weapon->Update();
		isAds = weapon->TakingAim();
	}

	//加速度と摩擦係数の処理
	float accel = status.accel;
	float friction = status.friction;


	//しゃがんでいる場合、加速しにくく滑りやすい
	if (crouch) {
		accel *= 0.08f;
		friction += 0.08f;
		if (VSize(velocity) > 0.18f) {
			if (slidingCT <= 0.0f&&onGround) {
				velocity = VScale(velocity, 2.5f);
				slidingCT = 5.0f;
			}
		}
	}

	//武器の速度減衰率を適用
	if (weapon) {
		if(isAds)
			accel *= weapon->GetSpec().adsDampingRatio;
		else
			accel *= weapon->GetSpec().hasDampingRatio;
	}


	if(!crouch) {
		if (!isAds) {
			if (running) {
				accel += 0.02f;
				friction -= 0.05f;
			}
		}
		
		//ジャンプの入力で瞬間的に上に加速
		if (InputManager::GetIns().IsActionHold(ActionID::JUMP)&&onGround){
			velocity.y += 0.25f;
		}
	}

	//空中にいる場合
	if (!onGround) {
		//時間で下に加速
		velocity.y += -0.008f * dt60;
		//加速しにくく慣性を残す
		accel *= 0.1f;
		friction = 0.98f;
	}

	//速度を変更
	//入力方向に加速
	velocity = VAdd(velocity, VScale(moveVec, accel * dt60));
	float finalfriction = std::pow(friction, dt60);
	//摩擦分減速
	velocity.x *= finalfriction;
	velocity.z *= finalfriction;

	float slopelimit = 0.6f;
	float radius = status.width;		//プレイヤーの半径
	int currentgroundPoly = -1;
	bool hitGround = false;
	//移動予定の座標
	VECTOR nextPos = VAdd(position, VScale(velocity, dt60));

	//床との判定
	//プレイヤーが落下、止まっている時のみ床の判定をする
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
			VECTOR start = VAdd(basePos, VGet(0, radius+0.1f, 0));
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
				hitGround = true;
			}
			currentgroundPoly = hitPolyIndex;
	}

	//壁との判定
	//プレイヤーのカプセル
	VECTOR capBottom = VAdd(nextPos, VGet(0, radius+0.1f, 0));
	VECTOR capTop = VAdd(nextPos, VGet(0, status.height - radius, 0));

	//プレイヤーのカプセルとぶつかっているモデルのポリゴンを取得
	MV1_COLL_RESULT_POLY_DIM wallHitDim = MV1CollCheck_Capsule(stageHandle, -1, capBottom, capTop, radius);

	//ぶつかっているポリゴンの数だけループ
	for (int i = 0; i < wallHitDim.HitNum; i++) {
		//当たっているポリゴンの法線ベクトルを取得
		VECTOR normal = wallHitDim.Dim[i].Normal;

		if (normal.y >= slopelimit) continue;

		float polyCenterY = (wallHitDim.Dim[i].Position[0].y + wallHitDim.Dim[i].Position[1].y + wallHitDim.Dim[i].Position[2].y) / 3.0f;

		float checkY = polyCenterY;
		if (checkY < capBottom.y)checkY = capBottom.y;
		if (checkY > capTop.y)checkY = capTop.y;
		//カプセルの下を基準
		VECTOR checkPos = VGet(capBottom.x,checkY,capBottom.z);

		//カプセルの中心から壁の面の最短距離
		float distance = VDot(VSub(checkPos, wallHitDim.Dim[i].Position[0]), normal);
		//距離が半径より短い時
		if (distance < radius && distance > -radius) {
			//めり込んだ分だけ押し出す
			float pushOver = radius - distance;

			VECTOR horizontalNormal = VGet(normal.x, 0.0f, normal.z);
			if (VSize(horizontalNormal) > 0.001f) {
				horizontalNormal = VNorm(horizontalNormal);
				nextPos.x += horizontalNormal.x * pushOver;
				nextPos.z += horizontalNormal.z * pushOver;

				capBottom.x = nextPos.x;
				capBottom.z = nextPos.z;
				capTop.x = nextPos.x;
				capTop.z = nextPos.z;
				float dotVec = velocity.x * horizontalNormal.x + velocity.z * horizontalNormal.z;
				if (dotVec < 0.0f) {
					velocity.x -= horizontalNormal.x * dotVec;
					velocity.z -= horizontalNormal.z * dotVec;
				}
			}
		}
	}
	//当たり判定で使ったメモリを解放
	MV1CollResultPolyDimTerminate(wallHitDim);
	//============================================================================================

	//位置反映
	onGround = hitGround;
	position = nextPos;

	Debug::Watch("X", position.x);
	Debug::Watch("Y", position.y);
	Debug::Watch("Z", position.z);
	//カメラ位置の更新,反映
	VECTOR camPos = position;

	//目標のカメラの高さ
	float targetcamHeight = crouch ? CROUCH_EYE_HEIGHT : EYE_HEIGHT;

	//現在のカメラの高さを目標に徐々に近づける
	float lerp = 1.0f - std::pow(1.0f - 0.1f, dt60);
	if (targetcamHeight > camHeight) {
		camHeight += abs(camHeight - targetcamHeight) * lerp;
		if (targetcamHeight < camHeight) {
			camHeight = targetcamHeight;
		}
	}if (targetcamHeight < camHeight) {
		camHeight -= abs(camHeight - targetcamHeight) * lerp;
		if (targetcamHeight > camHeight) {
			camHeight = targetcamHeight;
		}
	}
	//高さを適用
	camPos.y += camHeight;

	//視野角を変える処理
	float speed = VSize(velocity);
	float speedRate = speed / 0.75f;
	if (speedRate > 1.0f) speedRate = 1.0f;
	//速度に応じて視野角を広げる
	float targetFov = BASE_FOV + (MAX_FOV - BASE_FOV) * speedRate;
	if (weapon) {
		//覗いていれば武器のズームを適用
		if (isAds)
			targetFov = weapon->GetSpec().adsFov * DX_PI_F / 180;
	}
	//徐々に目標の視野角に近づける
	fov += (targetFov - fov) * lerp;

	//揺れをonにしている、覗いていない、着地時
	if (headBob && onGround&&!isAds) {
		float speed = VSize(velocity);
		//一定のスピードを維持している間(歩いている間)
		if(!(crouch&&speed>0.06f))
		if (speed > 0.01f) {
			//sin波でカメラを上下させる
			bobbingTimer += speed * 1.3f * dt60;
			float bobbingOffset = sinf(bobbingTimer) * 0.05f;
			camPos.y += bobbingOffset;
		}

		//揺らさなければ少しずつ元に戻す
		else {
			bobbingTimer *= 0.7f;
		}
	}

	//適用
	cam->Update(camPos);
	cam->Move(fov);
}

void Player::Draw() {}


void Player::AddRecoil(float y,float p){

	//覗いていないとき、しゃがんでいるとき反動を1/2にする
	if (!isAds||crouch) {
		y /= 2;
		p /= 2;
	}
	//反動をつける
	cam->AddAngle(y, p);
}

VECTOR Player::GetCamDirection() {
	return cam->GetLookDirection();
}