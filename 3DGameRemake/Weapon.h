#pragma once
#include "DxLib.h"
#include "Time.h"
#include "BaseProjectile.h"
#include "ProjectileManager.h"
#include "Character.h"
#include "InputManager.h"

class Camera;

//武器の状態を表す列挙型
enum class WeaponState {
	IDLE,
	RELOADING
};

/// <summary>
/// 武器を管理するクラス
/// </summary>
class Weapon {
protected:
	//性能
	GunStatus spec;		//武器スペック
	//状態
	WeaponState currentState; //現在の武器ステート
	int ammo;			 //現在の弾数
	int reserveAmmo;	 //現在の予備弾数
	bool infinite;

	float reloadCT;	     //リロード中タイマー     
	float fireCT;		 //次の射撃までのタイマー
	float equipCT;
	bool aim;

	float adsWeight;

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
	Weapon(const GunStatus _spec);
	virtual ~Weapon();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 射撃の入力を得る
	/// </summary>
	/// <param name="user">射手</param>
	/// <param name="direction">射撃方向のベクトル</param>
	void FireInput(Character& user, VECTOR direction);

	/// <summary>
	/// ADSの入力を得る
	/// </summary>
	void AdsInput();

	/// <summary>
	/// リロードの入力を得る
	/// </summary>
	void ReloadInput();

	/// <summary>
	/// 射撃処理
	/// </summary>
	/// <param name="user">射手</param>
	/// <param name="direction">射撃方向のベクトル</param>
	void Fire(Character& user, VECTOR direction);

	/// <summary>
	/// ADSの処理
	/// </summary>
	void Ads();
	
	/// <summary>
	/// リロードの処理
	/// </summary>
	void Reload();

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw(VECTOR basePos, VECTOR forward, VECTOR right, VECTOR up, bool isAds, bool isFPP);

	bool CanFire() const {
		return (fireCT <= 0 && currentState == WeaponState::IDLE && ammo > 0);
	}
		
	bool CanReload() const {
		return  ((infinite || reserveAmmo > 0) && ammo < spec.magAmmo && currentState == WeaponState::IDLE );
	}

	
	bool Reloading()const {
		return currentState == WeaponState::RELOADING;
	}

	bool TakingAim()const {
		return aim;
	}
	

	void CancelAds() {
		aim = false;
	}

	/// <summary>
	/// リロード中のリロードキャンセル処理
	/// </summary>
	void CancelReload() {
		if (currentState == WeaponState::RELOADING) {
			reloadCT = 0.0f;
			currentState = WeaponState::IDLE;
		}
	}

	void SetInfinite(bool flag) { infinite = flag; }
	void AddReserveAmmo(int amount) {
		reserveAmmo += amount;
	}

	bool IsSameType(const GunStatus& newspec) {
		return(spec.id == newspec.id);
	}
	const GunStatus& GetSpec() const { return spec; }
	int GetAmmo() const { return ammo; }
	int GetReserveAmmo() const { return reserveAmmo; }
	bool IsInfinite() const { return infinite; }
	
	void OnEquip() {
		equipCT = 0.15f;
	}
};