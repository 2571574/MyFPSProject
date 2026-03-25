#include "Player.h"
#include "Debug.h"
#include "ItemManager.h"
#include "ConfigManager.h"
#include "SoundManager.h"

#include <cmath>

namespace {
	constexpr float CROUCH_ACCEL_RATE = 0.08f;    
	constexpr float CROUCH_FRICTION_ADD = 0.08f;   
	constexpr float SLIDE_MIN_SPEED = 0.18f;        
	constexpr float SLIDE_BOOST = 2.5f;          
	constexpr float SLIDE_COOLDOWN = 5.0f;

	constexpr float RUN_ACCEL_ADD = 0.02f;        
	constexpr float RUN_FRICTION_SUB = 0.05f;
	constexpr float JUMP_POWER = 0.25f;

	constexpr float AIR_ACCEL_RATE = 0.1f;      
	constexpr float AIR_FRICTION = 0.98f;     

	constexpr float FOV_LERP_RATE = 0.1f;        
	constexpr float FOV_SPEED_BASE = 0.75f;
	constexpr float MAX_FOV = 110.0f * DX_PI_F / 180;

	constexpr float BOBBING_SPEED_MULT = 1.3f;  
	constexpr float BOBBING_AMPLITUDE = 0.05f;   
	constexpr float BOBBING_DECAY = 0.7f;       
	constexpr float BOBBING_MIN_SPEED = 0.01f;    
	constexpr float BOBBING_CROUCH_MIN_SPEED = 0.06f;

	constexpr float DROP_ITEM_Y_OFFSET = 0.4f;
}
Player::Player(VECTOR pos, Camera* camera, PlayMode mode)
	: Character(pos, CHARA_STATUS::PLAYER)
	, cam(camera)
	, stageHandle(-1)
	, forwardVec({ 0,0,0 })
	, rightVec({ 0,0,0 })
	, fov(0)
	, slidingCT(0.0f)
	, isAds(false)
	, running(false)
	, headBob(false)
	, bobbingTimer(0)
	, currentWeaponIndex(0)
	, currentMode(mode)
	, lastCamYaw(0.0f)
	, lastCamPitch(0.0f)
	, moveDistance(0.0f)
	,currentSwayX(0.0f)
	,currentSwayY(0.0f)
	
{
	hud = std::make_unique<HUD>(this);

	slot.push_back(std::make_unique<Weapon>(PLAYER_GUN::PISTOL));
	if (currentMode == PlayMode::MODE_EASY) {
		maxWeaponSlot = 10;
		slot.push_back(std::make_unique<Weapon>(PLAYER_GUN::RIFLE));
		slot.push_back(std::make_unique<Weapon>(PLAYER_GUN::SNIPER));
		slot.push_back(std::make_unique<Weapon>(PLAYER_GUN::SMG));
		slot.push_back(std::make_unique<Weapon>(PLAYER_GUN::LAUNCHER));

		for (auto& w : slot) {
			w->SetInfinite(true);
		}
	}

	if (currentMode == PlayMode::MODE_NORMAL || currentMode == PlayMode::MODE_TUTORIAL) {
		maxWeaponSlot = 10;
	}
	if (currentMode == PlayMode::MODE_HARD) {
		maxWeaponSlot = 2;
	}
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
	Weapon* currentWeapon = GetWeapon();
	if (crouch)running = false;
	//武器の更新と入力を検知
	if (currentWeapon) {
		currentWeapon->ReloadInput(*this);
		currentWeapon->FireInput(*this, GetCamDirection());
		if (running) {
			currentWeapon->CancelAds();
		}
		else{
			currentWeapon->AdsInput();
		}
		currentWeapon->Update(*this);
		isAds = currentWeapon->TakingAim();
	}

	//加速度と摩擦係数の処理
	float accel = status.accel;
	float friction = status.friction;


	//しゃがんでいる場合、加速しにくく滑りやすい
	if (crouch) {
		accel *= CROUCH_ACCEL_RATE;
		friction += CROUCH_FRICTION_ADD;
		if (VSize(velocity) > SLIDE_MIN_SPEED) {
			if (slidingCT <= 0.0f&&onGround) {
				velocity = VScale(velocity, SLIDE_BOOST);
				slidingCT = SLIDE_COOLDOWN;
			}
		}
	}

	//武器の速度減衰率を適用
	if (currentWeapon) {
		if(isAds)
			accel *= currentWeapon->GetSpec().adsDampingRatio;
		else
			accel *= currentWeapon->GetSpec().hasDampingRatio;
	}


	if(!crouch) {
		if (!isAds) {
			if (running) {
				accel += RUN_ACCEL_ADD;
				friction -= RUN_FRICTION_SUB;
			}
		}
		
		//ジャンプの入力で瞬間的に上に加速
		if (InputManager::GetIns().IsActionHold(ActionID::JUMP)&&onGround){
			velocity.y += JUMP_POWER;
		}
	}

	//空中にいる場合
	if (!onGround) {
		//加速しにくく慣性を残す
		accel *= AIR_ACCEL_RATE;
		friction = AIR_FRICTION;
	}


	velocity = VAdd(velocity, VScale(moveVec, accel * dt60));
	float finalFriction = std::pow(friction, dt60);
	velocity.x *= finalFriction;
	velocity.z *= finalFriction;
	
	UpdatePhysics(stageHandle);

	if (!slot.empty()) {
		int next = currentWeaponIndex;

		if (InputManager::GetIns().IsActionTrigger(ActionID::WEAPON_NEXT)) {
			next++;
			if (next >= slot.size())next = 0;
		}
		if (InputManager::GetIns().IsActionTrigger(ActionID::WEAPON_PREV)) {
			next--;
			if (next < 0)next = slot.size() - 1;
		}

		if (next != currentWeaponIndex) {
			SwitchWeapon(next);
		}
	}

	//カメラ位置の更新,反映
	VECTOR camPos = position;

	//目標のカメラの高さ
	float targetcamHeight = crouch ? status.crouchEyeHeight:status.eyeHeight;
	float targetBodyHeight = crouch ? status.crouchHeight :status.height;
	//現在のカメラの高さを目標に徐々に近づける
	float lerp = 1.0f - std::pow(1.0f - FOV_LERP_RATE, dt60);

	currentEyeHeight += (targetcamHeight - currentEyeHeight) * lerp;
	currentHeight += (targetBodyHeight - currentHeight) * lerp;
	
	//高さを適用
	camPos.y += currentEyeHeight;
	
	//視野角を変える処理
	float baseFov = ConfigManager::GetIns().Settings().basefov * DX_PI_F / 180;
	float speed = VSize(velocity);
	float speedRate = speed / FOV_SPEED_BASE;
	if (speedRate > 1.0f) speedRate = 1.0f;
	//速度に応じて視野角を広げる
	float targetFov = baseFov + (MAX_FOV - baseFov) * speedRate;
	if (currentWeapon) {
		//覗いていれば武器のズームを適用
		if (isAds)
			targetFov = currentWeapon->GetSpec().adsFov * DX_PI_F / 180;
	}
	//徐々に目標の視野角に近づける
	fov += (targetFov - fov) * lerp;

	headBob = ConfigManager::GetIns().Settings().headbob;
	//揺れをonにしている、覗いていない、着地時
	if (headBob && onGround&&!isAds) {
		float speed = VSize(velocity);
		//一定のスピードを維持している間(歩いている間)
		if(!(crouch&&speed>BOBBING_CROUCH_MIN_SPEED))
		if (speed > BOBBING_MIN_SPEED) {
			//sin波でカメラを上下させる
			bobbingTimer += speed * BOBBING_SPEED_MULT * dt60;
			float bobbingOffset = sinf(bobbingTimer) * BOBBING_AMPLITUDE;
			camPos.y += bobbingOffset;
		}

		//揺らさなければ少しずつ元に戻す
		else {
			bobbingTimer *= BOBBING_DECAY;
		}
	}

	//適用
	cam->Update(camPos);
	cam->Move(fov);

	float currentYaw = cam->GetYaw();
	float currentPitch = cam->GetPitch();

	float deltaYaw = currentYaw - lastCamYaw;

	if (deltaYaw > 180.0f)deltaYaw -= 360.0f;
	if (deltaYaw < -180.0f)deltaYaw += 360.0f;

	float deltaPitch = currentPitch - lastCamPitch;

	constexpr float SWAY_MULTIPLIER = 0.02f;
	constexpr float MAX_SWAY = 0.5f;

	float targetSwayX = -deltaYaw * SWAY_MULTIPLIER;
	float targetSwayY = deltaPitch * SWAY_MULTIPLIER;

	if (targetSwayX > MAX_SWAY) targetSwayX = MAX_SWAY;
	if (targetSwayX < -MAX_SWAY) targetSwayX = -MAX_SWAY;
	if (targetSwayY > MAX_SWAY) targetSwayY = MAX_SWAY;
	if (targetSwayY < -MAX_SWAY) targetSwayY = -MAX_SWAY;

	constexpr float SWAY_LERP_SPEED = 0.2f; 
	float swayLerp = 1.0f - std::pow(1.0f - SWAY_LERP_SPEED, dt60);
	currentSwayX += (targetSwayX - currentSwayX) * swayLerp;
	currentSwayY += (targetSwayY - currentSwayY) * swayLerp;

	lastCamYaw = currentYaw;
	lastCamPitch = currentPitch;
	hud->Update();

	Debug::Watch("X", position.x);
	Debug::Watch("Y", position.y);
	Debug::Watch("Z", position.z);

	if (cam) {
		VECTOR up = VGet(0.0f, 1.0f, 0.0f); // 基本的な上方向ベクトル
		SoundManager::GetIns().UpdateListener(cam->GetPos(), cam->GetLookDirection(), up);
	}

	UpdateFootstep();
}

void Player::Draw() {
	Weapon* currentWeapon = GetWeapon();
	if (currentWeapon) {
		VECTOR forward = cam->GetLookDirection();
		VECTOR hforward, right;
		cam->GetForwardVec(hforward, right);
		VECTOR up = VNorm(VCross(forward, right));
		right = VNorm(VCross(up, forward));

		VECTOR drawPos = cam->GetPos();

		if (!isAds) {
			float bobAmp = 0.015f;
			float bobX = cosf(bobbingTimer * 0.5f) * bobAmp;
			float bobY = sinf(bobbingTimer) * bobAmp;

			drawPos = VAdd(drawPos, VScale(right, bobX));
			drawPos = VAdd(drawPos, VScale(up, bobY));
		}

		float swayX = isAds ? currentSwayX * 0.1f : currentSwayX;
		float swayY = isAds ? currentSwayY * 0.1f : currentSwayY;

		drawPos = VAdd(drawPos, VScale(right, swayX));
		drawPos = VAdd(drawPos, VScale(up, swayY));
		currentWeapon->Draw(drawPos, forward, right, up, isAds, true);
	}
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


void Player::SwitchWeapon(int next) {
	if (slot.empty() || next == currentWeaponIndex)return;
	if (next < 0 || next >= slot.size())return;

	if (Weapon* current = GetWeapon()) {
		current->OnEquip();
		current->CancelAds();
		current->CancelReload();
	}

	currentWeaponIndex = next;
}

VECTOR Player::GetCamDirection()const {
	return cam->GetLookDirection();
}


bool Player::AddWeapon(std::unique_ptr<Weapon>& newWeapon) {
	if (!newWeapon)return false;
	for (auto& w : slot) {
		if (w->IsSameType(newWeapon->GetSpec())) {
				int getAmmo = newWeapon->GetAmmo() + newWeapon->GetReserveAmmo();
				w->AddReserveAmmo(getAmmo);
			newWeapon.reset();
			return true;
		}
	}

	if (slot.size() < maxWeaponSlot + 1) {
		slot.push_back(std::move(newWeapon));
		SwitchWeapon((int)slot.size() - 1);
		return true;
	}

	int dropIndex = (currentWeaponIndex == 0) ? 1 : currentWeaponIndex;
	
	if (dropIndex >= slot.size())return false;

	int totalAmmo = slot[dropIndex]->GetAmmo() + slot[dropIndex]->GetReserveAmmo();
	if (totalAmmo > 0) {
		VECTOR dropPos = VAdd(position, VGet(0.0f, DROP_ITEM_Y_OFFSET, 0.0f));

		auto dropItem = std::make_unique<WeaponItem>(dropPos, std::move(slot[dropIndex]));
		ItemManager::GetIns().SpawnDroppedItem(std::move(dropItem));
	}
	else {
		slot[dropIndex].reset();
	}

	slot[dropIndex] = std::move(newWeapon);

	SwitchWeapon(dropIndex);

	return true;
}

void Player::UpdateFootstep() {
	if (!onGround || !alive) return;

	float dt = Time::GetIns().GetDelta();
	float dt60 = dt * 60.0f;

	float speed = VSize(VGet(velocity.x, 0.0f, velocity.z));

	if (speed > 0.01f) {
		moveDistance += speed * dt60;
		
		constexpr float STEP_LENGTH = 4.0f;

		if (moveDistance >= STEP_LENGTH && !crouch) {
			SoundManager::GetIns().PlaySE("Resource/Sound/footstep.ogg");
			moveDistance -= STEP_LENGTH;
		}
	}
	else {
		moveDistance = 0.0f;
	}
}