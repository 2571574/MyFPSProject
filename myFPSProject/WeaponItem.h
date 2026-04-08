#pragma once
#include"DxLib.h"
#include"Weapon.h"
#include<memory>

/// <summary>
/// ドロップアイテム１つを管理するクラス
/// </summary>
class WeaponItem
{
private:
	VECTOR position;	//座標
	std::unique_ptr<Weapon> droppedWeapon;	//アイテム情報
	float bobbingTimer;	//縦揺れのタイマー
	bool alive;		//生存タグ

	int fontItemAmmo = -1;	//残弾数のフォント
	int fontItemName = -1;	//武器名のフォント
public:
	WeaponItem(VECTOR pos, std::unique_ptr<Weapon> weapon);
	~WeaponItem() = default;

	void Update();
	void Draw();

	/// <summary>
	/// 落ちている武器を拾う。
	/// </summary>
	/// <returns></returns>
	std::unique_ptr<Weapon>PickUp() {
		alive = false;
		return std::move(droppedWeapon);
	}

	VECTOR GetPos()const { return position; }
	bool IsAlive()const { return alive; }
	const GunStatus* GetSpec()const {
		if (droppedWeapon)return &droppedWeapon->GetSpec();
		return nullptr;
	}
};

