#include "Player.h"
#include "Debug.h"
#include "ItemManager.h"
#include "ConfigManager.h"
#include "SoundManager.h"
#include "Param/Chara.h"
#include "Param/Global.h"
#include "Param/System.h"

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
	, currentSwayX(0.0f)
	, currentSwayY(0.0f)
{
	hud = std::make_unique<HUD>(this);

	slot.push_back(std::make_unique<Weapon>(PLAYER_GUN::PISTOL));
	if (currentMode == PlayMode::MODE_EASY) {
		maxWeaponSlot = Chara::Player::MAX_WEAPON_SLOT_EASY_NORMAL;
		slot.push_back(std::make_unique<Weapon>(PLAYER_GUN::RIFLE));
		slot.push_back(std::make_unique<Weapon>(PLAYER_GUN::SNIPER));
		slot.push_back(std::make_unique<Weapon>(PLAYER_GUN::SMG));
		slot.push_back(std::make_unique<Weapon>(PLAYER_GUN::LAUNCHER));

		for (auto& w : slot) {
			w->SetInfinite(true);
		}
	}

	if (currentMode == PlayMode::MODE_NORMAL || currentMode == PlayMode::MODE_TUTORIAL) {
		maxWeaponSlot = Chara::Player::MAX_WEAPON_SLOT_EASY_NORMAL;
	}
	if (currentMode == PlayMode::MODE_HARD) {
		maxWeaponSlot = Chara::Player::MAX_WEAPON_SLOT_HARD;
	}
}

Player::~Player() {}

void Player::Update() {
	ClearTargeted();
	float dt = Time::GetIns().GetDelta();
	float dt60 = Global::Math::FPS_BASE * dt;
	if (slidingCT > 0.0f) {
		slidingCT -= dt;
		if (slidingCT <= 0.0f) {
			slidingCT = 0.0f;
		}
	}
	// 移動入力を取得
	VECTOR moveVec = VGet(0.0f, 0.0f, 0.0f);
	cam->GetForwardVec(forwardVec, rightVec);
	if (InputManager::GetIns().IsActionHold(ActionID::MOVE_FORWARD)) moveVec = VAdd(moveVec, forwardVec);
	if (InputManager::GetIns().IsActionHold(ActionID::MOVE_BACK)) moveVec = VSub(moveVec, forwardVec);
	if (InputManager::GetIns().IsActionHold(ActionID::MOVE_LEFT)) moveVec = VSub(moveVec, rightVec);
	if (InputManager::GetIns().IsActionHold(ActionID::MOVE_RIGHT)) moveVec = VAdd(moveVec, rightVec);

	// コントローラーの移動入力を取得
	int LX, LY;
	CheckKey::GetIns().GetLeftStick(LX, LY);
	float right = LX / Chara::Player::STICK_INPUT_SCALE;
	float forward = -LY / Chara::Player::STICK_INPUT_SCALE;
	moveVec = VAdd(moveVec, VScale(forwardVec, forward));
	moveVec = VAdd(moveVec, VScale(rightVec, right));

	if (VSize(moveVec) > Chara::Player::INPUT_VECTOR) {
		moveVec = VNorm(moveVec);
	}

	crouch = InputManager::GetIns().IsActionHold(ActionID::CROUCH);
	running = InputManager::GetIns().IsActionHold(ActionID::RUN);
	Weapon* currentWeapon = GetWeapon();
	if (crouch) running = false;

	// 武器の更新と入力を検知
	if (currentWeapon) {
		currentWeapon->ReloadInput(*this);
		currentWeapon->FireInput(*this, GetCamDirection());
		if (running) {
			currentWeapon->CancelAds();
		}
		else {
			currentWeapon->AdsInput();
		}
		currentWeapon->Update(*this);
		isAds = currentWeapon->TakingAim();
	}

	// 加速度と摩擦係数の処理
	float accel = status.accel;
	float friction = status.friction;

	// しゃがんでいる場合、加速しにくく滑りやすい
	if (crouch) {
		accel *= Chara::Player::CROUCH_ACCEL_RATE;
		friction += Chara::Player::CROUCH_FRICTION_ADD;
		if (VSize(velocity) > Chara::Player::SLIDE_MIN_SPEED) {
			if (slidingCT <= 0.0f && onGround) {
				velocity = VScale(velocity, Chara::Player::SLIDE_BOOST);
				slidingCT = Chara::Player::SLIDE_COOLDOWN;
			}
		}
	}

	// 武器の速度減衰率を適用
	if (currentWeapon) {
		if (isAds) accel *= currentWeapon->GetSpec().adsDampingRatio;
		else accel *= currentWeapon->GetSpec().hasDampingRatio;
	}

	if (!crouch) {
		if (!isAds) {
			if (running) {
				accel += Chara::Player::RUN_ACCEL_ADD;
				friction -= Chara::Player::RUN_FRICTION_SUB;
			}
		}

		// ジャンプの入力で瞬間的に上に加速
		if (InputManager::GetIns().IsActionHold(ActionID::JUMP) && onGround) {
			velocity.y += Chara::Player::JUMP_POWER;
		}
	}

	// 空中にいる場合
	if (!onGround) {
		accel *= Chara::Player::AIR_ACCEL_RATE;
		friction = Chara::Base::AIR_KB_FRICTION;
	}

	//加速と摩擦の適用
	velocity = VAdd(velocity, VScale(moveVec, accel * dt60));
	float finalFriction = std::pow(friction, dt60);
	velocity.x *= finalFriction;
	velocity.z *= finalFriction;

	//物理演算の更新
	UpdatePhysics(stageHandle);

	//　武器切替の検知
	if (!slot.empty()) {
		int next = currentWeaponIndex;

		if (InputManager::GetIns().IsActionTrigger(ActionID::WEAPON_NEXT)) {
			next++;
			if (next >= slot.size()) next = 0;
		}
		if (InputManager::GetIns().IsActionTrigger(ActionID::WEAPON_PREV)) {
			next--;
			if (next < 0) next = slot.size() - 1;
		}

		if (next != currentWeaponIndex) {
			SwitchWeapon(next);
		}
	}

	// カメラ位置の更新,反映
	VECTOR camPos = position;

	float targetcamHeight = crouch ? status.crouchEyeHeight : status.eyeHeight;
	float targetBodyHeight = crouch ? status.crouchHeight : status.height;
	float lerp = 1.0f - std::pow(1.0f - Chara::Player::FOV_LERP_RATE, dt60);

	currentEyeHeight += (targetcamHeight - currentEyeHeight) * lerp;
	currentHeight += (targetBodyHeight - currentHeight) * lerp;

	camPos.y += currentEyeHeight;

	// 視野角を変える処理
	float baseFov = ConfigManager::GetIns().Settings().basefov * Global::Math::DEG_TO_RAD;
	float speed = VSize(velocity);
	float speedRate = speed / Chara::Player::FOV_SPEED_BASE;
	if (speedRate > 1.0f) speedRate = 1.0f;

	float targetFov = baseFov + (Chara::Player::MAX_FOV - baseFov) * speedRate;
	if (currentWeapon) {
		if (isAds) targetFov = currentWeapon->GetSpec().adsFov * Global::Math::DEG_TO_RAD;
	}
	fov += (targetFov - fov) * lerp;

	headBob = ConfigManager::GetIns().Settings().headbob;
	if (headBob && onGround && !isAds) {
		if (!(crouch && speed > Chara::Player::BOBBING_CROUCH_MIN_SPEED)) {
			if (speed > Chara::Base::MOVEMENT_MIN) {
				bobbingTimer += speed * Chara::Player::BOBBING_SPEED_MULT * dt60;
				float bobbingOffset = sinf(bobbingTimer) * Chara::Player::BOBBING_AMPLITUDE;
				camPos.y += bobbingOffset;
			}
			else {
				bobbingTimer *= Chara::Player::BOBBING_DECAY;
			}
		}
	}

	cam->Update(camPos);
	cam->Move(fov);

	//武器の揺れの計算
	float currentYaw = cam->GetYaw();
	float currentPitch = cam->GetPitch();

	float deltaYaw = currentYaw - lastCamYaw;

	if (deltaYaw > Chara::Player::YAW_HALF_TURN_DEG) deltaYaw -= Chara::Player::YAW_FULL_TURN_DEG;
	if (deltaYaw < -Chara::Player::YAW_HALF_TURN_DEG) deltaYaw += Chara::Player::YAW_FULL_TURN_DEG;

	float deltaPitch = currentPitch - lastCamPitch;

	float targetSwayX = -deltaYaw * Chara::Player::SWAY_MULTIPLIER;
	float targetSwayY = deltaPitch * Chara::Player::SWAY_MULTIPLIER;

	if (targetSwayX > Chara::Player::MAX_SWAY) targetSwayX = Chara::Player::MAX_SWAY;
	if (targetSwayX < -Chara::Player::MAX_SWAY) targetSwayX = -Chara::Player::MAX_SWAY;
	if (targetSwayY > Chara::Player::MAX_SWAY) targetSwayY = Chara::Player::MAX_SWAY;
	if (targetSwayY < -Chara::Player::MAX_SWAY) targetSwayY = -Chara::Player::MAX_SWAY;

	float swayLerp = 1.0f - std::pow(1.0f - Chara::Player::SWAY_LERP_SPEED, dt60);
	currentSwayX += (targetSwayX - currentSwayX) * swayLerp;
	currentSwayY += (targetSwayY - currentSwayY) * swayLerp;

	lastCamYaw = currentYaw;
	lastCamPitch = currentPitch;
	hud->Update();

	//音の更新
	if (cam) {
		VECTOR forward = cam->GetLookDirection();
		VECTOR up = VGet(0.0f, 1.0f, 0.0f);

		SoundManager::GetIns().UpdateListener(cam->GetPos(), forward, up);
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
			float bobX = cosf(bobbingTimer * Chara::Player::BOBBING_WEAPON_FREQUENCY_MULT) * Chara::Player::BOBBING_WEAPON_AMPLITUDE;
			float bobY = sinf(bobbingTimer) * Chara::Player::BOBBING_WEAPON_AMPLITUDE;

			drawPos = VAdd(drawPos, VScale(right, bobX));
			drawPos = VAdd(drawPos, VScale(up, bobY));
		}

		float swayX = isAds ? currentSwayX * Chara::Player::ADS_SWAY_SCALE : currentSwayX;
		float swayY = isAds ? currentSwayY * Chara::Player::ADS_SWAY_SCALE : currentSwayY;

		drawPos = VAdd(drawPos, VScale(right, swayX));
		drawPos = VAdd(drawPos, VScale(up, swayY));
		currentWeapon->Draw(drawPos, forward, right, up, isAds, true);
	}
	if (hud) hud->Draw();
}


void Player::AddRecoil(float y,float p){

	//覗いていないとき、しゃがんでいるとき反動を1/2にする
	if (!isAds || crouch) {
		y *= Chara::Player::RECOIL_HIPFIRE_MULTIPLIER;
		p *= Chara::Player::RECOIL_HIPFIRE_MULTIPLIER;
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
	if (!newWeapon) return false;
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
	if (dropIndex >= slot.size()) return false;

	int totalAmmo = slot[dropIndex]->GetAmmo() + slot[dropIndex]->GetReserveAmmo();
	if (totalAmmo > 0) {
		VECTOR dropPos = VAdd(position, VGet(0.0f, Chara::Player::DROP_ITEM_Y_OFFSET, 0.0f));

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
	float dt60 = dt * Global::Math::FPS_BASE;

	float speed = VSize(VGet(velocity.x, 0.0f, velocity.z));

	if (speed > Chara::Base::MOVEMENT_MIN) {
		moveDistance += speed * dt60;

		if (moveDistance >= Chara::Player::STEP_LENGTH && !crouch) {
			SoundManager::GetIns().Play3DSE
			("Resource/Sound/footstep.wav",position, Chara::Base::FOOTSTEP_SOUND_RADIUS);
			moveDistance -= Chara::Player::STEP_LENGTH;
		}
	}
	else {
		moveDistance = 0.0f;
	}
}