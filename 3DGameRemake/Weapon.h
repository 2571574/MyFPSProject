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
	bool reloadcanceled;
	bool aim;
	//演出用
	int gunModelHandle;	 //銃のモデルハンドル
	int bulletModelHandle; //弾のモデルハンドル
	int effectHandle;	 //発射時のエフェクト
	int soundHandle;	 //サウンドハンドル

	//弾速のある弾を生成する　user=射手　direction=射撃方向のベクトル
	virtual void FireProjectile(Character& user, VECTOR direction);

	//即着の弾の処理　中でヒットスキャンの判定を行う　user=射手　direction=射撃方向のベクトル
	virtual void FireHitScan(Character& user, VECTOR direction);		//ヒットスキャンの発射

	//射撃後の処理　クールタイムのセットや弾数の減少など　user=射手
	void Fired(Character& user);
	
public:
	Weapon(const GunStatus _spec) :spec(_spec), ammo(_spec.magAmmo), reserveAmmo(_spec.bagAmmo),
		reloadCT(0)
		, fireCT(0)
		, reloading(false)
		, aim(false)
		, reloadcanceled(false)
		, gunModelHandle(-1)
		, bulletModelHandle(-1)
		, effectHandle(-1)
		, soundHandle(-1)
	{
		if (reserveAmmo == 0) {
			infinite = true;
		}
		else infinite = false;
	}

	
	virtual ~Weapon() {}

	/// <summary>
	/// 更新処理
	/// </summary>
	virtual void Update();

	/// <summary>
	/// 射撃の入力を得る
	/// </summary>
	/// <param name="user">射手</param>
	/// <param name="direction">射撃方向のベクトル</param>
	virtual void FireInput(Character& user, VECTOR direction);

	/// <summary>
	/// ADSの入力を得る
	/// </summary>
	virtual void AdsInput();

	/// <summary>
	/// リロードの入力を得る
	/// </summary>
	virtual void ReloadInput();

	/// <summary>
	/// 射撃処理
	/// </summary>
	/// <param name="user">射手</param>
	/// <param name="direction">射撃方向のベクトル</param>
	virtual void Fire(Character& user, VECTOR direction);

	/// <summary>
	/// ADSの処理
	/// </summary>
	virtual void Ads();
	
	/// <summary>
	/// リロードの処理
	/// </summary>
	virtual void Reload();

	/// <summary>
	/// 描画処理
	/// </summary>
	virtual void Draw();

	bool CanFire() const {
		return (fireCT <= 0 && !reloading && ammo > 0);
	}
		
	bool CanReload() const {
		return  ((infinite || reserveAmmo > 0) && ammo < spec.magAmmo && !reloading );
	}

	
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
		reloadcanceled = true;
	}

	bool IsReloadCanceled() const {
		return reloadcanceled;
	}

	void SetReloadCanceled() {
		reloadcanceled = false;
	}
	//getter
	GunStatus GetSpec() const { return spec; }
	int GetAmmo() const { return ammo; }
	int GetReserveAmmo() const { return reserveAmmo; }
	bool IsInfinite() const { return infinite; }
};