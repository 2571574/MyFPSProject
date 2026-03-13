#pragma once
#include"DxLib.h"
#include"Weapon.h"
#include<memory>
class WeaponItem
{
private:
	VECTOR position;
	std::unique_ptr<Weapon> droppedWeapon;
	float bobbingTimer;
	bool alive;

public:
	WeaponItem(VECTOR pos, std::unique_ptr<Weapon> weapon);
	~WeaponItem() = default;

	void Update();
	void Draw();

	VECTOR GetPos()const { return position; }
	bool IsAlive()const { return alive; }

	std::unique_ptr<Weapon>PickUp() {
		alive = false;
		return std::move(droppedWeapon);
	}

	const GunStatus* GetSpec()const {
		if (droppedWeapon)return &droppedWeapon->GetSpec();
		return nullptr;
	}
};

