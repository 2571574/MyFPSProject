#pragma once
#include"DxLib.h"
#include"Weapon.h"
#include<memory>
class WeaponItem
{
private:
	VECTOR position;
	std::unique_ptr<Weapon> droppedWeapon;	//アイテム情報
	float bobbingTimer;	//縦揺れのタイマー
	bool alive;

	int fontItemAmmo = -1;
	int fontItemName = -1;
public:
	WeaponItem(VECTOR pos, std::unique_ptr<Weapon> weapon);
	~WeaponItem() = default;

	void Update();
	void Draw();


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

