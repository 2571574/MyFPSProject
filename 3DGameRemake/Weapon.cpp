#include"Weapon.h"
#include"Character.h"
#include "CollisionManager.h"

#include <memory>

namespace {
	constexpr float SPREAD_RANDOM_PRECISION = 1000.0f;
	constexpr float RAY_MAX_DISTANCE = 100.0f;
	constexpr float RECOIL_RANDOM_PRECISION = 100.0f;
}

Weapon::Weapon(const GunStatus _spec)
	: spec(_spec)
	, ammo(_spec.magAmmo)
	, reserveAmmo(_spec.bagAmmo)
	, infinite(_spec.isInfinite)
	, currentState(WeaponState::IDLE)
	, reloadCT(0)
	, fireCT(0)
	, aim(false)
	, gunModelHandle(-1)
	, bulletModelHandle(-1)
	, effectHandle(-1)
	, soundHandle(-1){}

/*武器の更新*/
void Weapon::Update() {
	//リロード、射撃後のタイマーを減らす
	float delta = Time::GetIns().GetDelta();
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
		FireHitScan(user, dir);
	}
	else {
		FireProjectile(user, dir);
	}
	//射撃後の処理
	Fired(user);
}

/*弾速のある弾を発射する関数*/
void Weapon::FireProjectile(Character& user, VECTOR direction) {
	//射撃位置の取得
	VECTOR userEyePos = VAdd(user.GetPos(), VGet(0, user.GetCurrentEyeHeight(), 0));
	VECTOR right = VNorm(VCross(VGet(0.0f, 1.0f, 0.0f), direction));				//右のベクトル
	VECTOR up = VNorm(VCross(direction, right));					//上のベクトル
	VECTOR offset = aim ? VGet(0.0f, 0.0f, 1.0f) : spec.muzzleOffset;				//銃口のオフセット

	//回転に応じてオフセット分ずらす
	VECTOR finalOffset;
	finalOffset = VAdd(VScale(right, offset.x), VScale(up, offset.y));
	finalOffset = VAdd(finalOffset, VScale(direction, offset.z));
	VECTOR spawnPos = VAdd(userEyePos, finalOffset);

	VECTOR targetPos = VAdd(userEyePos, VScale(direction, 100.0f));
	VECTOR lastDir = VNorm(VSub(targetPos, spawnPos));

	//弾の生成
	auto p = std::make_unique<BaseProjectile>(spawnPos, user.GetID(), spec, lastDir);
	ProjectileManager::GetIns().Spawn(std::move(p));
}

/*即着の弾を発射する関数*/
void Weapon::FireHitScan(Character& user, VECTOR direction) {
	//射撃位置の取得
	VECTOR userEyePos = VAdd(user.GetPos(), VGet(0, user.GetCurrentEyeHeight(), 0));
	VECTOR right = VNorm(VCross(VGet(0.0f, 1.0f, 0.0f), direction));
	VECTOR up = VNorm(VCross(direction, right));
	VECTOR offset = aim ? VGet(0.0f, 0.0f, 1.0f) : spec.muzzleOffset;
	//回転に応じてオフセット分ずらす
	VECTOR finalOffset;
	finalOffset = VAdd(VScale(right, offset.x), VScale(up, offset.y));
	finalOffset = VAdd(finalOffset, VScale(direction, offset.z));
	VECTOR spawnPos = VAdd(userEyePos, finalOffset);

	VECTOR targetPos = VAdd(userEyePos, VScale(direction, 100.0f));
	VECTOR lastDir = VNorm(VSub(targetPos, spawnPos));

	//始点と終点をセット
	VECTOR start = spawnPos;
	VECTOR end = VAdd(start, VScale(lastDir, spec.range));
	//始点から終点までで当たったか判定する
	HitInfo hit = CollisionManager::GetIns().CheckHitScan(start, end, user.GetID());
	if (hit.character != nullptr) {
		user.HitRecord(hit.isHeadShot);
		int lastdamage = hit.isHeadShot ? spec.damage * 2 : spec.damage;
		if (hit.isHeadShot)Debug::Log("Headshot");
		else Debug::Log("hit");
		hit.character->OnHit(lastdamage,spec.id);	//当たった場合の被弾処理
	}
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
void Weapon::Draw() {

}