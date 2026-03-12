#include "Player.h"
#include "Debug.h"
#include "cmath"


Player::Player(VECTOR pos, Camera* camera)
	: Character(pos, CHARA_STATUS::PLAYER)
	, cam(camera)
	, stageHandle(-1)
	, forwardVec({ 0,0,0 })
	, rightVec({ 0,0,0 })
	, weapon(std::make_unique<Weapon>(PLAYER_GUN::LAUNCHER))
	, fov(0)
	,slidingCT(0.0f)
	, isAds(false)
	,running(false)
	, headBob(true)
	, bobbingTimer(0)
	
{
	hud = std::make_unique<HUD>(this);
}


Player::~Player() {
}


void Player::Update() {
	ClearTargeted();
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
		//加速しにくく慣性を残す
		accel *= 0.1f;
		friction = 0.98f;
	}


	velocity = VAdd(velocity, VScale(moveVec, accel * dt60));
	float finalFriction = std::pow(friction, dt60);
	velocity.x *= finalFriction;
	velocity.z *= finalFriction;
	
	UpdatePhysics(stageHandle);
	//カメラ位置の更新,反映
	VECTOR camPos = position;

	//目標のカメラの高さ
	float targetcamHeight = crouch ? status.crouchEyeHeight:status.eyeHeight;
	float targetBodyHeight = crouch ? status.crouchHeight :status.height;
	//現在のカメラの高さを目標に徐々に近づける
	float lerp = 1.0f - std::pow(1.0f - 0.1f, dt60);

	currentEyeHeight += (targetcamHeight - currentEyeHeight) * lerp;
	currentHeight += (targetBodyHeight - currentHeight) * lerp;
	
	//高さを適用
	camPos.y += currentEyeHeight;
	
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

	hud->Update();

	Debug::Watch("X", position.x);
	Debug::Watch("Y", position.y);
	Debug::Watch("Z", position.z);
}

void Player::Draw() {
	if (hud)hud->Draw();
}


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
