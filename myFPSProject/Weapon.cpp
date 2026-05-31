#include "Weapon.h"
#include "Param/Global.h"
#include "Param/Item.h"
#include "Param/Visual.h"
#include "Character.h"
#include "Camera.h"
#include "CollisionManager.h"
#include "ResourceManager.h"
#include "EffectManager.h"
#include "SoundManager.h"

#include <memory>

Weapon::Weapon(const GunStatus _spec)
	: spec(_spec)
	, ammo(_spec.magAmmo)
	, reserveAmmo(_spec.bagAmmo)
	, infinite(_spec.isInfinite)
	, currentState(WeaponState::IDLE)
	, reloadCT(0)
	, fireCT(0)
	, equipTimer(Item::Weapon::EQUIP_TIME)
	, aim(false)
	, adsWeight(0.0f)
	, gunModelHandle(-1)
	, reloadEndPlayed(false)
{
	if (!spec.visual.modelPath.empty()) {
		gunModelHandle = ResourceManager::GetIns().DuplicateModel(spec.visual.modelPath);
	}
	else {
		gunModelHandle = -1;
	}
}

Weapon::~Weapon() {
	if (gunModelHandle != -1) {
		MV1DeleteModel(gunModelHandle);
	}
}

void Weapon::Update(Character& user) {
	float delta = Time::GetIns().GetDelta();
	float dt60 = delta * Global::Math::FPS_BASE;

	float lerpRate = 1.0f - std::pow(1.0f - Item::Weapon::ADS_SPEED, dt60);
	float targetAds = aim ? 1.0f : 0.0f;

	adsWeight += (targetAds - adsWeight) * lerpRate;
	if (reloadCT > 0) {
		reloadCT -= delta;

		if (currentState == WeaponState::RELOADING && !reloadEndPlayed && reloadCT <= Item::Weapon::RELOAD_END_SOUND_THRESHOLD) {

			if (!spec.visual.reloadEndSoundPath.empty()) {

				if (user.GetID() == TEAMID::ID_FRIENDLY) {
					SoundManager::GetIns().PlaySE(spec.visual.reloadEndSoundPath);
				}

				else {
					SoundManager::GetIns().Play3DSE(spec.visual.reloadEndSoundPath, user.GetPos(), Item::Weapon::SOUND_RADIUS_NORMAL);
				}

			}
			reloadEndPlayed = true;
		}

		if (reloadCT <= 0)
			reloadCT = 0;
	}

	if (fireCT > 0) {
		fireCT -= delta;

		if (fireCT <= 0) fireCT = 0;
	}

	if (equipTimer > 0) {
		equipTimer -= delta;
		if (equipTimer <= 0) equipTimer = 0;
	}

	// リロードで弾を補充
	if (currentState == WeaponState::RELOADING) {

		if (reloadCT <= 0.0f) {
			int need = spec.magAmmo - ammo;
			int add;

			if (infinite)
				add = need;
			else
				add = (reserveAmmo < need) ? reserveAmmo : need;

			ammo += add;
			reserveAmmo -= add;
			currentState = WeaponState::IDLE;
		}
	}
}

void Weapon::Fired(Character& user) {
	//弾数を減らす、クールタイムをセット
	if (ammo > 0) ammo--;
	fireCT = 1.0f / spec.fireRate;

	//反動の処理
	float recoilP = spec.recoil;
	int randMax = static_cast<int>(spec.recoil * Item::Weapon::RECOIL_RANDOM_PRECISION);
	float recoilY = (randMax > 0) ? (((float)GetRand(randMax) / Item::Weapon::RECOIL_RANDOM_PRECISION) - (spec.recoil / 2.0f)) : 0.0f;
	user.AddRecoil(recoilY, recoilP);
}


void Weapon::FireInput(Character& user, VECTOR direction) {
	if (ammo <= 0 && currentState == WeaponState::IDLE) {
		if (InputManager::GetIns().IsActionTrigger(ActionID::FIRE)) {
			SoundManager::GetIns().Play3DSE("Resource/Sound/Fireempty.wav", user.GetPos(), Item::Weapon::SOUND_RADIUS_NORMAL);
		}
	}

	if (spec.fullAuto) {
		if (InputManager::GetIns().IsActionHold(ActionID::FIRE)) {
			Fire(user, direction);
		}
	}

	else {
		if (InputManager::GetIns().IsActionTrigger(ActionID::FIRE)) {
			Fire(user, direction);
		}
	}
}


void Weapon::ReloadInput(Character& user) {
	if (InputManager::GetIns().IsActionTrigger(ActionID::RELOAD)) {
		Reload(user);
	}
}


void Weapon::AdsInput() {
	if (currentState == WeaponState::RELOADING) {
		aim = false;
		return;
	}

	aim = InputManager::GetIns().IsActionHold(ActionID::ADS);

	if (aim) {
		Ads();
	}
}


void Weapon::Fire(Character& user, VECTOR direction) {
	if (!CanFire())return;

	if (!spec.visual.fireSoundPath.empty()) {
		SoundManager::GetIns().Play3DSE(spec.visual.fireSoundPath, user.GetPos(), Item::Weapon::SOUND_RADIUS_LOUD);
	}

	user.ShotRecord();

	//ADSとしゃがみによって拡散を変える
	VECTOR dir = VNorm(direction);
	float currentSpread = aim ? spec.adsSpread : spec.spread;

	if (user.GetCrouching()) {
		currentSpread *= Item::Weapon::CROUCH_SPREAD_MULTIPLIER;
	}

	VECTOR right = VNorm(VCross(VGet(0.0f, 1.0f, 0.0f), direction));
	VECTOR up = VNorm(VCross(direction, right));
	
	float randX = ((float)GetRand(Item::Weapon::SPREAD_RANDOM_RANGE) - Item::Weapon::SPREAD_RANDOM_PRECISION) / Item::Weapon::SPREAD_RANDOM_PRECISION;
	float randY = ((float)GetRand(Item::Weapon::SPREAD_RANDOM_RANGE) - Item::Weapon::SPREAD_RANDOM_PRECISION) / Item::Weapon::SPREAD_RANDOM_PRECISION;

	dir = VAdd(direction, VScale(right, randX * currentSpread));
	dir = VAdd(dir, VScale(up, randY * currentSpread));
	dir = VNorm(dir);

	//specのhitscanによって判別
	if (spec.hitscan) {
		FireHitScan(user, direction, dir);
	}

	else {
		FireProjectile(user, direction, dir);
	}

	//射撃後の処理
	Fired(user);
}

void Weapon::FireProjectile(Character& user, VECTOR baseDir, VECTOR shootDir) {
	VECTOR userEyePos = VAdd(user.GetPos(), VGet(0, user.GetCurrentEyeHeight(), 0));
	VECTOR right = VNorm(VCross(VGet(0.0f, 1.0f, 0.0f), baseDir));
	VECTOR up = VNorm(VCross(baseDir, right));				
	VECTOR fireOffset = VAdd(VScale(spec.muzzleOffset, 1.0f - adsWeight), VScale(spec.adsMuzzleOffset, adsWeight));	

	VECTOR fireFinal;
	fireFinal = VAdd(VScale(right, fireOffset.x), VScale(up, fireOffset.y));
	fireFinal = VAdd(fireFinal, VScale(baseDir, fireOffset.z));
	VECTOR spawnPos = VAdd(userEyePos, fireFinal);

	VECTOR targetPos = VAdd(userEyePos, VScale(shootDir, Item::Weapon::RAY_MAX_DISTANCE));
	VECTOR lastDir = VNorm(VSub(targetPos, spawnPos));

	//エフェクト用の銃口位置を計算
	VECTOR visualOffset = VAdd(VScale(spec.visual.drawMuzzleOffset, 1.0f - adsWeight), VScale(spec.visual.drawAdsMuzzleOffset, adsWeight));
	VECTOR visualFinal;
	visualFinal = VAdd(VScale(right, visualOffset.x), VScale(up, visualOffset.y));
	visualFinal = VAdd(visualFinal, VScale(baseDir, visualOffset.z));
	VECTOR visualMuzzlePos = VAdd(userEyePos, visualFinal);

	EffectManager::GetIns().CreateMuzzleFlash(visualMuzzlePos, baseDir, Visual::Effect::MUZZLE_FLASH_BASE_SIZE);

	//弾の生成
	auto p = std::make_unique<BaseProjectile>(spawnPos, user.GetID(), spec, lastDir);
	ProjectileManager::GetIns().Spawn(std::move(p));
}


void Weapon::FireHitScan(Character& user, VECTOR baseDir, VECTOR shootDir) {
	VECTOR userEyePos = VAdd(user.GetPos(), VGet(0, user.GetCurrentEyeHeight(), 0));
	VECTOR right = VNorm(VCross(VGet(0.0f, 1.0f, 0.0f), baseDir));
	VECTOR up = VNorm(VCross(baseDir, right));

	VECTOR fireOffset = VAdd(VScale(spec.muzzleOffset, 1.0f - adsWeight), VScale(spec.adsMuzzleOffset, adsWeight));
	VECTOR fireFinal;
	fireFinal = VAdd(VScale(right, fireOffset.x), VScale(up, fireOffset.y));
	fireFinal = VAdd(fireFinal, VScale(baseDir, fireOffset.z));
	VECTOR spawnPos = VAdd(userEyePos, fireFinal);

	//画面中央の遠くを目標点とし、銃口からそこまでのベクトルを計算する
	VECTOR targetPos = VAdd(userEyePos, VScale(shootDir, Item::Weapon::RAY_MAX_DISTANCE));
	VECTOR lastDir = VNorm(VSub(targetPos, spawnPos));

	//エフェクト用の銃口位置を計算
	VECTOR visualOffset = VAdd(VScale(spec.visual.drawMuzzleOffset, 1.0f - adsWeight), VScale(spec.visual.drawAdsMuzzleOffset, adsWeight));
	VECTOR visualFinal;
	visualFinal = VAdd(VScale(right, visualOffset.x), VScale(up, visualOffset.y));
	visualFinal = VAdd(visualFinal, VScale(baseDir, visualOffset.z));
	VECTOR visualMuzzlePos = VAdd(userEyePos, visualFinal);

	EffectManager::GetIns().CreateMuzzleFlash(visualMuzzlePos, baseDir, Visual::Effect::MUZZLE_FLASH_BASE_SIZE);

	VECTOR start = spawnPos;
	VECTOR end = VAdd(start, VScale(lastDir, spec.range));

	VECTOR actualEnd = end;

	//始点から終点までで当たったか判定する
	HitInfo hit = CollisionManager::GetIns().CheckHitScan(start, end, user.GetID());

	if (hit.character != nullptr) {
		int lastdamage = hit.isHeadShot ? spec.damage * Item::Weapon::HEADSHOT_MULTIPLIER : spec.damage;
		hit.character->OnHit(lastdamage, spec.id);
		bool isKill = (hit.character->GetHP() <= 0);
		user.HitRecord(hit.isHeadShot, isKill);

		EffectManager::GetIns().CreateHitEffect(hit.hitPos, hit.hitNormal, true);
	}

	else if (hit.isWallHit) {
		EffectManager::GetIns().CreateHitEffect(hit.hitPos, hit.hitNormal, false);
	}

	int trailColor = GetColor(Visual::Effect::COLOR_TRAIL_FRIENDLY.r, Visual::Effect::COLOR_TRAIL_FRIENDLY.g, Visual::Effect::COLOR_TRAIL_FRIENDLY.b);
	EffectManager::GetIns().CreateHitScanTrail(visualMuzzlePos, actualEnd, trailColor);
}


void Weapon::Reload(Character& user) {
	if (!CanReload()) return;
	currentState = WeaponState::RELOADING;
	reloadCT = spec.reloadTime;
	reloadEndPlayed = false;

	if (!spec.visual.reloadSoundPath.empty()) {

		if (user.GetID() == TEAMID::ID_FRIENDLY) {
			SoundManager::GetIns().PlaySE(spec.visual.reloadSoundPath);
		}

		else {
			SoundManager::GetIns().Play3DSE(spec.visual.reloadSoundPath, user.GetPos(), Item::Weapon::SOUND_RADIUS_NORMAL);
		}
	}
}

void Weapon::Ads() {}

void Weapon::Draw(VECTOR basePos, VECTOR forward, VECTOR right, VECTOR up, bool isAds, bool isFPP) {
	if (gunModelHandle == -1)return;
	//スケール設定
	float s = spec.visual.scale;
	MATRIX scaleMat = MGetScale(VGet(s, s, s));

	//ベースの回転
	MATRIX localRot = MGetRotY(Visual::WeaponAnim::MODEL_BASE_ROTATION_Y);
	VECTOR animOffset = VGet(0.0f, 0.0f, 0.0f);
	MATRIX animRot = MGetIdent();

	//装備アニメーション
	if (equipTimer > 0.0f) {
		//取り出しアニメーション
		float progress = 1.0f - (equipTimer / Item::Weapon::EQUIP_TIME);
		float easeOut = sinf(progress * DX_PI_F / 2.0f);

		s *= (Visual::WeaponAnim::ANIM_EQUIP_START_SCALE + (1.0f - Visual::WeaponAnim::ANIM_EQUIP_START_SCALE) * easeOut);

		animOffset = VGet(0.0f, Visual::WeaponAnim::ANIM_EQUIP_OFFSET_Y * (1.0f - easeOut), Visual::WeaponAnim::ANIM_EQUIP_OFFSET_Z * (1.0f - easeOut));
		animRot = MGetRotX(Visual::WeaponAnim::ANIM_EQUIP_ROT_X * (1.0f - easeOut));
	}
	
	//リロードアニメーション
	else if (currentState == WeaponState::RELOADING && spec.reloadTime > 0.0f) {
		float progress = 1.0f - (reloadCT / spec.reloadTime);

		//武器ごとにアニメーションを分ける
		switch (spec.id) {

		//ピストルとSMG
		case WeaponID::PIS:
		case WeaponID::SMG:
		{
			//銃を持ち上げる
			if (progress < Visual::WeaponAnim::ANIM_RELOAD_SMG_PHASE1) {
				float t = progress / Visual::WeaponAnim::ANIM_RELOAD_SMG_PHASE1;
				float ease = sinf(t * DX_PI_F / 2.0f);

				animOffset = VGet(0.0f, Visual::WeaponAnim::ANIM_RELOAD_SMG_WINDUP_Y * ease, (Visual::WeaponAnim::ANIM_RELOAD_SMG_PULLBACK_Z * 0.3f) * ease);
				animRot = MGetRotX(Global::Math::MATH_PI_QUARTER * ease);
			}

			//引き寄せて回転させる
			else if (progress < Visual::WeaponAnim::ANIM_RELOAD_SMG_PHASE2) {
				float t = (progress - Visual::WeaponAnim::ANIM_RELOAD_SMG_PHASE1) / (Visual::WeaponAnim::ANIM_RELOAD_SMG_PHASE2 - Visual::WeaponAnim::ANIM_RELOAD_SMG_PHASE1);

				float zEase = sinf(t * DX_PI_F);
				float currentZ = (Visual::WeaponAnim::ANIM_RELOAD_SMG_PULLBACK_Z * 0.3f) * (1.0f - t) + Visual::WeaponAnim::ANIM_RELOAD_SMG_PULLBACK_Z * zEase;

				animOffset = VGet(0.0f, Visual::WeaponAnim::ANIM_RELOAD_SMG_WINDUP_Y * (1.0f - t), currentZ);

				float targetRot = -DX_PI_F * 2.0f * Visual::WeaponAnim::ANIM_RELOAD_SMG_SPINS;
				float currentRot = Global::Math::MATH_PI_QUARTER * (1.0f - t) + targetRot * t;

				animRot = MGetRotX(currentRot);
			}

			//振り下ろして元の位置に戻す
			else {
				float t = (progress - Visual::WeaponAnim::ANIM_RELOAD_SMG_PHASE2) / (1.0f - Visual::WeaponAnim::ANIM_RELOAD_SMG_PHASE2);
				float ease = sinf(t * DX_PI_F);

				animOffset = VGet(0.0f, Visual::WeaponAnim::ANIM_RELOAD_SMG_OVERSHOOT_Y * ease, 0.0f);
				animRot = MGetRotX(Visual::WeaponAnim::ANIM_RELOAD_SMG_OVERSHOOT_ROT_X * ease);
			}

			break;
		}

		//ランチャー
		case WeaponID::LR:
		{


			//上に持っていく
			if (progress < Visual::WeaponAnim::ANIM_RELOAD_LR_PHASE1) {
				float t = progress / Visual::WeaponAnim::ANIM_RELOAD_LR_PHASE1;
				float easeIn = 1.0f - cosf(t * DX_PI_F / 2.0f);

				animOffset = VGet(Visual::WeaponAnim::ANIM_RELOAD_LR_SHOULDER_X * easeIn, Visual::WeaponAnim::ANIM_RELOAD_LR_SHOULDER_Y * easeIn, Visual::WeaponAnim::ANIM_RELOAD_LR_SHOULDER_Z * easeIn);
				animRot = MGetRotX(Visual::WeaponAnim::ANIM_RELOAD_LR_ROT_X * easeIn);
			}

			else if (progress < Visual::WeaponAnim::ANIM_RELOAD_LR_PHASE2) {
				animOffset = VGet(Visual::WeaponAnim::ANIM_RELOAD_LR_SHOULDER_X, Visual::WeaponAnim::ANIM_RELOAD_LR_SHOULDER_Y, Visual::WeaponAnim::ANIM_RELOAD_LR_SHOULDER_Z);
				animRot = MGetRotX(Visual::WeaponAnim::ANIM_RELOAD_LR_ROT_X);
			}

			//元の位置に戻す
			else {
				float t = (progress - Visual::WeaponAnim::ANIM_RELOAD_LR_PHASE2) / (1.0f - Visual::WeaponAnim::ANIM_RELOAD_LR_PHASE2);
				float easeOut = sinf(t * DX_PI_F / 2.0f);

				animOffset = VGet(
					Visual::WeaponAnim::ANIM_RELOAD_LR_SHOULDER_X * (1.0f - easeOut),
					Visual::WeaponAnim::ANIM_RELOAD_LR_SHOULDER_Y * (1.0f - easeOut),
					Visual::WeaponAnim::ANIM_RELOAD_LR_SHOULDER_Z * (1.0f - easeOut)
				);
				animRot = MGetRotX(Visual::WeaponAnim::ANIM_RELOAD_LR_ROT_X * (1.0f - easeOut));
			}
			break;
		}

		//デフォルト(アサルトやスナイパーなど)
		default:
			float transitionRatio = Visual::WeaponAnim::ANIM_RELOAD_DEFAULT_TRANSITION;
			float lerpFactor = 0.0f;

			if (progress < transitionRatio) {
				lerpFactor = sinf((progress / transitionRatio) * DX_PI_F / 2.0f);
			}

			else if (progress > 1.0f - transitionRatio) {
				lerpFactor = sinf(((1.0f - progress) / transitionRatio) * DX_PI_F / 2.0f);
			}

			else {
				lerpFactor = 1.0f;
			}

			float targetRotY = (-DX_PI_F / 2.0f) * lerpFactor;

			animRot = MGetRotY(targetRotY * lerpFactor);
			break;
		}
	}


	MATRIX rot = MGetIdent();
	rot.m[0][0] = right.x;		rot.m[0][1] = right.y;		rot.m[0][2] = right.z;
	rot.m[1][0] = up.x;			rot.m[1][1] = up.y;			rot.m[1][2] = up.z;
	rot.m[2][0] = forward.x;	rot.m[2][1] = forward.y;	rot.m[2][2] = forward.z;

	VECTOR worldOffset = VAdd(VScale(spec.visual.drawOffset, 1.0f - adsWeight), VScale(spec.visual.adsDrawOffset, adsWeight));
	worldOffset = VAdd(worldOffset, animOffset);
	VECTOR drawPos = basePos;
	drawPos = VAdd(drawPos, VScale(right, worldOffset.x));
	drawPos = VAdd(drawPos, VScale(up, worldOffset.y));
	drawPos = VAdd(drawPos, VScale(forward, worldOffset.z));
	MATRIX transMat = MGetTranslate(drawPos);

	MATRIX worldMat = MMult(MMult(MMult(MMult(scaleMat, localRot), animRot), rot), transMat);


	if (isAds && spec.id == WeaponID::SR && isFPP) {
		int scopeGraph = ResourceManager::GetIns().GetGraph("Resource/SniperScope.png");
		if (scopeGraph != -1) {
			DrawExtendGraph(0, 0, System::Window::WINDOW_WIDTH, System::Window::WINDOW_HEIGHT, scopeGraph, TRUE);
		}
	}

	else {
		DxLib::MV1SetMatrix(gunModelHandle, worldMat);

		if (isFPP) {
			ClearDrawScreenZBuffer();
		}
		MV1DrawModel(gunModelHandle);
	}
}