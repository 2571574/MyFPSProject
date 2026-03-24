#include"Weapon.h"
#include"Character.h"
#include "CollisionManager.h"
#include "ResourceManager.h"
#include "Camera.h"
#include "Parameter.h"
#include "EffectManager.h"

#include <memory>

namespace {
	constexpr float SPREAD_RANDOM_PRECISION = 1000.0f;
	constexpr float RAY_MAX_DISTANCE = 100.0f;
	constexpr float RECOIL_RANDOM_PRECISION = 100.0f;
	constexpr float EQUIP_TIME = 0.15f;
}

Weapon::Weapon(const GunStatus _spec)
	: spec(_spec)
	, ammo(_spec.magAmmo)
	, reserveAmmo(_spec.bagAmmo)
	, infinite(_spec.isInfinite)
	, currentState(WeaponState::IDLE)
	, reloadCT(0)
	, fireCT(0)
	, equipCT(EQUIP_TIME)
	, aim(false)
	, adsWeight(0.0f)
	, gunModelHandle(-1)
	, bulletModelHandle(-1)
	, effectHandle(-1)
	, soundHandle(-1)
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

/*武器の更新*/
void Weapon::Update() {
	//リロード、射撃後のタイマーを減らす
	float delta = Time::GetIns().GetDelta();
	float dt60 = delta * 60.0f;
	constexpr float ADS_SPEED = 0.6f;
	float lerpRate = 1.0f - std::pow(1.0f - ADS_SPEED, dt60);
	float targetAds = aim ? 1.0f : 0.0f;

	adsWeight += (targetAds - adsWeight) * lerpRate;
	if (reloadCT > 0) {
		reloadCT -= delta;
		if (reloadCT <= 0)
			reloadCT = 0;
	}
	if (fireCT > 0) {
		fireCT -= delta;
		if (fireCT <= 0)
			fireCT = 0;
	}
	if (equipCT > 0) {
		equipCT -= delta;
		if (equipCT <= 0) equipCT = 0;
	}

	// リロード中かつタイマーが尽きたときだけ弾を補充
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
	int randMax = static_cast<int>(spec.recoil * RECOIL_RANDOM_PRECISION);
	float recoilY = (randMax>0) ?(((float)GetRand(randMax) / RECOIL_RANDOM_PRECISION) - (spec.recoil/2.0)) : 0.0f;
	user.AddRecoil(recoilY, recoilP);
}

/*射撃の入力を得る関数*/
void Weapon::FireInput(Character& user, VECTOR direction) {
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

/*リロードの入力を得る関数*/
void Weapon::ReloadInput() {
	if (InputManager::GetIns().IsActionTrigger(ActionID::RELOAD)) {
		Reload();
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

/*射撃処理*/
void Weapon::Fire(Character& user, VECTOR direction) {
	if (!CanFire())return;		//撃てなかったらreturn
	user.ShotRecord();
	VECTOR dir = VNorm(direction);
	float currentSpread = aim ? spec.adsSpread : spec.spread;
	if (user.IsCrouching()) {
		currentSpread *= 0.7f;
	}
		VECTOR right = VNorm(VCross(VGet(0.0f, 1.0f, 0.0f), direction));
		VECTOR up = VNorm(VCross(direction, right));

		float randX = ((float)GetRand(2000) - SPREAD_RANDOM_PRECISION) / SPREAD_RANDOM_PRECISION;
		float randY = ((float)GetRand(2000) - SPREAD_RANDOM_PRECISION) / SPREAD_RANDOM_PRECISION;
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

/*弾速のある弾を発射する関数*/
void Weapon::FireProjectile(Character& user, VECTOR baseDir, VECTOR shootDir) {
	//射撃位置の取得
	VECTOR userEyePos = VAdd(user.GetPos(), VGet(0, user.GetCurrentEyeHeight(), 0));
	VECTOR right = VNorm(VCross(VGet(0.0f, 1.0f, 0.0f), baseDir));				//右のベクトル
	VECTOR up = VNorm(VCross(baseDir, right));					//上のベクトル
	VECTOR fireOffset = VAdd(VScale(spec.muzzleOffset, 1.0f - adsWeight), VScale(spec.adsMuzzleOffset, adsWeight));				//銃口のオフセット

	//回転に応じてオフセット分ずらす
	VECTOR fireFinal;
	fireFinal = VAdd(VScale(right, fireOffset.x), VScale(up, fireOffset.y));
	fireFinal = VAdd(fireFinal, VScale(baseDir, fireOffset.z));
	VECTOR spawnPos = VAdd(userEyePos, fireFinal);

	VECTOR targetPos = VAdd(userEyePos, VScale(shootDir, 100.0f));
	VECTOR lastDir = VNorm(VSub(targetPos, spawnPos));

	VECTOR visualOffset = VAdd(VScale(spec.visual.drawMuzzleOffset, 1.0f - adsWeight), VScale(spec.visual.drawAdsMuzzleOffset, adsWeight));
	VECTOR visualFinal;
	visualFinal = VAdd(VScale(right, visualOffset.x), VScale(up, visualOffset.y));
	visualFinal = VAdd(visualFinal, VScale(baseDir, visualOffset.z));
	VECTOR visualMuzzlePos = VAdd(userEyePos, visualFinal);

	EffectManager::GetIns().CreateMuzzleFlash(visualMuzzlePos, baseDir, 0.5f);

	//弾の生成
	auto p = std::make_unique<BaseProjectile>(spawnPos, user.GetID(), spec, lastDir);
	ProjectileManager::GetIns().Spawn(std::move(p));
}

/*即着の弾を発射する関数*/
void Weapon::FireHitScan(Character& user, VECTOR baseDir, VECTOR shootDir) {
	//射撃位置の取得
	VECTOR userEyePos = VAdd(user.GetPos(), VGet(0, user.GetCurrentEyeHeight(), 0));
	VECTOR right = VNorm(VCross(VGet(0.0f, 1.0f, 0.0f), baseDir));
	VECTOR up = VNorm(VCross(baseDir, right));

	//回転に応じてオフセット分ずらす
	VECTOR fireOffset = VAdd(VScale(spec.muzzleOffset, 1.0f - adsWeight), VScale(spec.adsMuzzleOffset, adsWeight));
	VECTOR fireFinal;
	fireFinal = VAdd(VScale(right, fireOffset.x), VScale(up, fireOffset.y));
	fireFinal = VAdd(fireFinal, VScale(baseDir, fireOffset.z));
	VECTOR spawnPos = VAdd(userEyePos, fireFinal);

	VECTOR targetPos = VAdd(userEyePos, VScale(shootDir, 100.0f));
	VECTOR lastDir = VNorm(VSub(targetPos, spawnPos));

	VECTOR visualOffset = VAdd(VScale(spec.visual.drawMuzzleOffset, 1.0f - adsWeight), VScale(spec.visual.drawAdsMuzzleOffset, adsWeight));
	VECTOR visualFinal;
	visualFinal = VAdd(VScale(right, visualOffset.x), VScale(up, visualOffset.y));
	visualFinal = VAdd(visualFinal, VScale(baseDir, visualOffset.z));
	VECTOR visualMuzzlePos = VAdd(userEyePos, visualFinal);

	EffectManager::GetIns().CreateMuzzleFlash(visualMuzzlePos, baseDir, 0.5f);

	//始点と終点をセット
	VECTOR start = spawnPos;
	VECTOR end = VAdd(start, VScale(lastDir, spec.range));

	VECTOR actualEnd = end;

	//始点から終点までで当たったか判定する
	HitInfo hit = CollisionManager::GetIns().CheckHitScan(start, end, user.GetID());
	if (hit.character != nullptr) {
		user.HitRecord(hit.isHeadShot);
		int lastdamage = hit.isHeadShot ? spec.damage * 2 : spec.damage;
		if (hit.isHeadShot)Debug::Log("Headshot");
		else Debug::Log("hit");
		hit.character->OnHit(lastdamage,spec.id);	//当たった場合の被弾処理
		Debug::Log("EffectSpawn true");
		EffectManager::GetIns().CreateHitEffect(hit.hitPos, hit.hitNormal, true);
	}
	else if (hit.isWallHit) {
		Debug::Log("EffectSpawn false");
		EffectManager::GetIns().CreateHitEffect(hit.hitPos, hit.hitNormal, false);
	}

	int trailColor = GetColor(200, 200, 200);
	EffectManager::GetIns().CreateHitScanTrail(visualMuzzlePos, actualEnd, trailColor);
}

/*リロード*/
void Weapon::Reload() {
	if (!CanReload()) return;	//できなければreturn
	currentState = WeaponState::RELOADING;
	reloadCT = spec.reloadTime;
}

void Weapon::Ads() {
}

//描画
void Weapon::Draw(VECTOR basePos, VECTOR forward, VECTOR right, VECTOR up, bool isAds, bool isFPP) {
	if (gunModelHandle == -1)return;
	float s = spec.visual.scale;
	MATRIX scaleMat = MGetScale(VGet(s, s, s));

	MATRIX localRot = MGetRotY(-DX_PI_F / 2.0f);

	VECTOR animOffset = VGet(0.0f, 0.0f, 0.0f);
	MATRIX animRot = MGetIdent();
	if (equipCT > 0.0f) {
		float progress = 1.0f - (equipCT / EQUIP_TIME);
		float easeOut = sinf(progress * DX_PI_F / 2.0f);

		// スケール: 0.5倍から1.0倍(元のサイズ)へ
		s *= (0.5f + 0.5f * easeOut);

		// 位置と回転: 下＆手前から定位置へ
		float startOffsetY = -0.6f;
		float startOffsetZ = -0.3f;
		float startRotX = DX_PI_F / 4.0f;

		animOffset = VGet(0.0f, startOffsetY * (1.0f - easeOut), startOffsetZ * (1.0f - easeOut));
		animRot = MGetRotX(startRotX * (1.0f - easeOut));
	}
	else if (currentState == WeaponState::RELOADING && spec.reloadTime > 0.0f) {
		float progress = 1.0f - (reloadCT / spec.reloadTime);	
		switch (spec.id) {
		case WeaponID::PIS:
		case WeaponID::SMG:
		{
			float phase1End = 0.15f;
			float phase2End = 0.85f;

			float windUpOffsetY = -0.1f;
			float windUpRotX = DX_PI_F / 4.0f;

			float spinRotations = 6.0f;
			float pullBackOffsetZ = -0.2f;

			float overshootOffsetY = 0.1f;
			float overshootRotX = -DX_PI_F / 8.0f;

			if (progress < phase1End) {
				float t = progress / phase1End;
				float ease = sinf(t * DX_PI_F / 2.0f);

				animOffset = VGet(0.0f, windUpOffsetY * ease, (pullBackOffsetZ * 0.3f) * ease);
				animRot = MGetRotX(windUpRotX * ease);
			}
			else if (progress < phase2End) {
				float t = (progress - phase1End) / (phase2End - phase1End);

				float zEase = sinf(t * DX_PI_F);
				float currentZ = (pullBackOffsetZ * 0.3f) * (1.0f - t) + pullBackOffsetZ * zEase;

				animOffset = VGet(0.0f, windUpOffsetY * (1.0f - t), currentZ);

				float targetRot = -DX_PI_F * 2.0f * spinRotations;
				float currentRot = windUpRotX * (1.0f - t) + targetRot * t;

				animRot = MGetRotX(currentRot);
			}
			else {
				float t = (progress - phase2End) / (1.0f - phase2End);
				float ease = sinf(t * DX_PI_F);

				animOffset = VGet(0.0f, overshootOffsetY * ease, 0.0f);
				animRot = MGetRotX(overshootRotX * ease);
			}
			break;
		}
		case WeaponID::LR:
		{
			float phase1End = 0.3f;
			float phase2End = 0.7f;

			float shoulderOffsetY = 0.8f;
			float shoulderOffsetX = 0.3f;
			float shoulderOffsetZ = -0.2f;

			float shoulderRotX = -DX_PI_F / 2.5f;

			if (progress < phase1End) {
				float t = progress / phase1End;
				float easeIn = 1.0f - cosf(t * DX_PI_F / 2.0f);

				animOffset = VGet(shoulderOffsetX * easeIn, shoulderOffsetY * easeIn, shoulderOffsetZ * easeIn);
				animRot = MGetRotX(shoulderRotX * easeIn);
			}
			else if (progress < phase2End) {
				animOffset = VGet(shoulderOffsetX, shoulderOffsetY, shoulderOffsetZ);
				animRot = MGetRotX(shoulderRotX);
			}
			else {
				float t = (progress - phase2End) / (1.0f - phase2End);
				float easeOut = sinf(t * DX_PI_F / 2.0f);

				animOffset = VGet(
					shoulderOffsetX * (1.0f - easeOut),
					shoulderOffsetY * (1.0f - easeOut),
					shoulderOffsetZ * (1.0f - easeOut)
				);
				animRot = MGetRotX(shoulderRotX * (1.0f - easeOut));
			}
			break;
		}
		default:
			float transitionRatio = 0.07f;
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
		if(scopeGraph != -1){
			DrawExtendGraph(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, scopeGraph, TRUE);
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