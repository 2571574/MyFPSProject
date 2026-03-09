#include"Weapon.h"
#include"EnemyManager.h"
#include <memory>

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
	if (reloading && reloadCT <= 0) {
		int need = spec.magAmmo - ammo;
		int add;
		if (infinite)
			add = need;
		else
			add = (reserveAmmo < need) ? reserveAmmo : need;
		ammo += add;
		reserveAmmo -= add;
		reloading = false;
	}
	Debug::Watch("reload", reloadCT);
	Debug::Watch("Aim", aim);
	Debug::Watch("ammo", ammo);
	Debug::Watch("reserve", reserveAmmo);
}

void Weapon::Fired(Character& user) {
	//弾数を減らす、クールタイムをセット
	if (ammo > 0) ammo--;
	fireCT = 1.0f / spec.fireRate;

	//反動の処理
	float recoilP = spec.recoil;
	float recoilY = ((float)GetRand(spec.recoil) - (spec.recoil/2));
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
	VECTOR dir = VNorm(direction);
	float currentSpread = aim ? spec.adsSpread : spec.spread;
		VECTOR right = VNorm(VCross(VGet(0.0f, 1.0f, 0.0f), direction));
		VECTOR up = VNorm(VCross(direction, right));
		float randX = ((float)GetRand(2000) - 1000.0f) / 1000.0f;
		float randY = ((float)GetRand(2000) - 1000.0f) / 1000.0f;
		Debug::Watch("right.x",right.x);
		Debug::Watch("right.z",right.z);
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
	VECTOR userEyePos = VAdd(user.GetPos(), VGet(0, user.GetCamHeight(), 0));
	VECTOR right = VNorm(VCross(VGet(0.0f, 1.0f, 0.0f), direction));				//右のベクトル
	VECTOR up = VNorm(VCross(direction, right));					//上のベクトル
	VECTOR offset = aim ? VGet(0.0f, 0.0f, 3.0f) : spec.muzzleOffset;				//銃口のオフセット

	//回転に応じてオフセット分ずらす
	VECTOR finalOffset;
	finalOffset = VAdd(VScale(right, offset.x), VScale(up, offset.y));
	finalOffset = VAdd(finalOffset, VScale(direction, offset.z));
	VECTOR spawnPos = VAdd(userEyePos, finalOffset);

	//弾の生成
	auto p = std::make_unique<BaseProjectile>(spawnPos, user.GetID(), spec, direction);
	ProjectileManager::GetIns().Spawn(std::move(p));
}

/*即着の弾を発射する関数*/
void Weapon::FireHitScan(Character& user, VECTOR direction) {
	//射撃位置の取得
	VECTOR userEyePos = VAdd(user.GetPos(), VGet(0, user.GetCamHeight(), 0));
	VECTOR right = VNorm(VCross(VGet(0.0f, 1.0f, 0.0f), direction));
	VECTOR up = VNorm(VCross(direction, right));
	VECTOR offset = aim ? VGet(0.0f, 0.0f, 3.0f) : spec.muzzleOffset;
	//回転に応じてオフセット分ずらす
	VECTOR finalOffset;
	finalOffset = VAdd(VScale(right, offset.x), VScale(up, offset.y));
	finalOffset = VAdd(finalOffset, VScale(direction, offset.z));
	VECTOR spawnPos = VAdd(userEyePos, finalOffset);

	//始点と終点をセット
	VECTOR start = spawnPos;
	VECTOR end = VAdd(start, VScale(direction, spec.range));
	DrawLine3D(start, end, GetColor(255, 255, 0));
	//始点から終点までで当たったか判定する
	Enemy* hitenemy = EnemyManager::GetIns().CheckHitScan(start, end, user.GetID());
	if (hitenemy != nullptr) {
		hitenemy->OnHit(spec.damage);	//当たった場合の被弾処理
	}
}

/*リロード*/
void Weapon::Reload() {
	if (!CanReload()) return;	//できなければreturn
	reloading = true;			
	reloadCT = spec.reloadTime;
}

void Weapon::Ads() {
}

//描画
void Weapon::Draw() {

}