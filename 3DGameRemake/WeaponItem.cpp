#include "WeaponItem.h"
#include "Time.h"
#include "TextManager.h"
#include "ItemManager.h"

#include <cmath>

namespace {
	constexpr float BOBBING_SPEED = 2.0f;
	constexpr float BOBBING_AMPLITUDE = 0.2f;
	constexpr float BASE_HEIGHT_OFFSET = 0.5f;
	constexpr float CUBE_HALF_SIZE = 0.25f;

	constexpr float UI_DISPLAY_DISTANCE = 15.0f;
	constexpr int UI_OFFSET_X = -20;
	constexpr int UI_OFFSET_Y_AMMO = -40;
	constexpr int UI_OFFSET_Y_NAME = -20;
}
WeaponItem::WeaponItem(VECTOR pos, std::unique_ptr<Weapon>weapon)
	:position(pos)
	,droppedWeapon(std::move(weapon))
	,bobbingTimer(0.0f)
	,alive(true){ }

void WeaponItem::Update() {
	float dt = Time::GetIns().GetDelta();
	bobbingTimer += dt * BOBBING_SPEED;
}

void WeaponItem::Draw() {
	if (!alive || !droppedWeapon)return;

	float yOffset = std::sinf(bobbingTimer) * BOBBING_AMPLITUDE;
	VECTOR drawPos = VAdd(position, VGet(0.0f, yOffset + BASE_HEIGHT_OFFSET, 0.0f));

	DrawCube3D(VAdd(drawPos, VGet(CUBE_HALF_SIZE, CUBE_HALF_SIZE, CUBE_HALF_SIZE)), VAdd(drawPos, VGet(-CUBE_HALF_SIZE, -CUBE_HALF_SIZE, -CUBE_HALF_SIZE)), GetColor(255, 255, 0), GetColor(255, 255, 0), TRUE);

	VECTOR camPos = ItemManager::GetIns().GetCamPos();
	float distance = VSize(VSub(camPos, drawPos));
	if (distance <= UI_DISPLAY_DISTANCE) {
		VECTOR screenPos = ConvWorldPosToScreenPos(drawPos);
		if (screenPos.z >= 0.0f && screenPos.z <= 1.0f) {
			int stageHandle = ItemManager::GetIns().GetStageHandle();
			MV1_COLL_RESULT_POLY hit = MV1CollCheck_Line(stageHandle, -1, camPos, drawPos);
			if (hit.HitFlag == 0) {
				const char* weaponName = TextManager::GetIns().GetWeaponName(droppedWeapon->GetSpec().id);

				int drawX = static_cast<int>(screenPos.x) + UI_OFFSET_X;
				int drawYAmmo = static_cast<int>(screenPos.y) + UI_OFFSET_Y_AMMO;
				int drawYName = static_cast<int>(screenPos.y) + UI_OFFSET_Y_NAME;

				DrawFormatString(drawX, drawYAmmo, GetColor(0, 0, 0), "AMMO: %d / %d", droppedWeapon->GetAmmo(), droppedWeapon->GetReserveAmmo());
				DrawFormatString(drawX, drawYName, GetColor(0, 0, 0), "%s", weaponName);
			}
		}
	}
}