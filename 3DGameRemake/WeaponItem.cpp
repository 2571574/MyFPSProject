#include "WeaponItem.h"
#include "Time.h"
#include "TextManager.h"
#include <cmath>

WeaponItem::WeaponItem(VECTOR pos, std::unique_ptr<Weapon>weapon)
	:position(pos)
	,droppedWeapon(std::move(weapon))
	,bobbingTimer(0.0f)
	,alive(true){ }

void WeaponItem::Update() {
	float dt = Time::GetIns().GetDelta();
	bobbingTimer += dt * 2.0f;
}

void WeaponItem::Draw() {
	if (!alive || !droppedWeapon)return;

	float yOffset = std::sinf(bobbingTimer) * 0.2f;
	VECTOR drawPos = VAdd(position, VGet(0.0f, yOffset + 0.5f, 0.0f));

	DrawCube3D(VAdd(drawPos, VGet(0.25f, 0.25f, 0.25f)), VAdd(drawPos, VGet(-0.25f, -0.25f, -0.25f)), GetColor(255, 255, 0), GetColor(255, 255, 0), TRUE);

	VECTOR screenPos = ConvWorldPosToScreenPos(drawPos);
	if (screenPos.z >= 0.0f && screenPos.z <= 1.0f) {
		const char* weaponName = TextManager::GetIns().GetWeaponName(droppedWeapon->GetSpec().id);
		DrawFormatString((int)screenPos.x - 20, (int)screenPos.y - 40, GetColor(0, 0, 0), "AMMO: %d / %d", droppedWeapon->GetAmmo(), droppedWeapon->GetReserveAmmo());
		DrawFormatString((int)screenPos.x - 20, (int)screenPos.y - 20, GetColor(0, 0, 0), "%s", weaponName);
	}
}