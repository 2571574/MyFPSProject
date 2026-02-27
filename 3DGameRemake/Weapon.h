#pragma once
#include "DxLib.h"
#include "Time.h"
#include "BaseProjectile.h"
#include "ProjectileManager.h"
#include "Character.h"
#include"Debug.h"
#include "InputManager.h"

/// <summary>
/// 武器を管理するクラス
/// </summary>
class Weapon {
protected:
	//性能
	GunStatus spec;		//武器スペック
	//状態
	int ammo;			 //現在の弾数
	int reserveAmmo;	 //現在の予備弾数
	bool infinite;

	float reloadCT;	     //リロード中タイマー     
	float fireCT;		 //次の射撃までのタイマー
	bool reloading;		 //リロード中
	bool aim;
	//演出用
	int gunModelHandle;	 //銃のモデルハンドル
	int bulletModelHandle; //弾のモデルハンドル
	int effectHandle;	 //発射時のエフェクト
	int soundHandle;	 //サウンドハンドル

	/// <summary>
	/// 弾速がある弾を発射する
	/// </summary>
	/// <param name="user"></param>
	/// <param name="direction"></param>
	virtual void FireProjectile(Character& user, VECTOR direction);		//プロジェクタイルの発射
	virtual void FireHitScan(Character& user, VECTOR direction);		//ヒットスキャンの発射

	//発射後の処理
	void Fired(Character& user);
	
public:
	Weapon(const GunStatus _spec) :spec(_spec), ammo(_spec.magAmmo), reserveAmmo(_spec.bagAmmo),				//コンストラクタ
		reloadCT(0), fireCT(0), reloading(false), aim(false), gunModelHandle(-1), bulletModelHandle(-1), effectHandle(-1), soundHandle(-1) {
		if (reserveAmmo == 0) {
			infinite = true;
		}
		else infinite = false;
	}

	//デストラクタ
	virtual ~Weapon() {}

	//更新
	virtual void Update();

	//入力処理
	virtual void FireInput(Character& user, VECTOR direction);	//射撃の入力を得る
	virtual void AdsInput();
	virtual void ReloadInput();	//リロードの入力を得る

	//射撃
	virtual void Fire(Character& user, VECTOR direction);

	virtual void Ads();
	//リロード
	virtual void Reload();

	//描画
	virtual void Draw();

	//撃てるか確認
	bool CanFire() const {
		return (fireCT <= 0 && !reloading && ammo > 0);
	}
	//リロードできるか確認
	bool CanReload() const {
		return  ((infinite || reserveAmmo > 0) && ammo < spec.magAmmo && !reloading );
	}

	//リロード中
	bool Reloading()const {
		return reloading;
	}

	bool TakingAim()const {
		return aim;
	}

	virtual void CancelAds() {
		aim = false;
	}

	virtual void CancelReload() {
		reloading = false;
		reloadCT = 0.0f;
	}
	//getter
	GunStatus GetSpec() const { return spec; }
	int GetAmmo() const { return ammo; }
	int GetReserveAmmo() const { return reserveAmmo; }
};