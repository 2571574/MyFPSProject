#include "WeaponItem.h"
#include "Time.h"
#include "TextManager.h"
#include "ItemManager.h"
#include "ResourceManager.h"
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

	constexpr int RING_SEGMENTS = 32;
	constexpr float RING_RADIUS_INNER = 0.6f;
	constexpr float RING_RADIUS_OUTER = 0.8f;
}
WeaponItem::WeaponItem(VECTOR pos, std::unique_ptr<Weapon>weapon)
	:position(pos)
	,droppedWeapon(std::move(weapon))
	,bobbingTimer(0.0f)
	,alive(true){ 
	fontItem = ResourceManager::GetIns().GetFont("Century Gothic", 20, 2);
}

void WeaponItem::Update() {
	float dt = Time::GetIns().GetDelta();
	bobbingTimer += dt * BOBBING_SPEED;
}

void WeaponItem::Draw() {
	if (!alive || !droppedWeapon)return;

	float yOffset = std::sinf(bobbingTimer) * BOBBING_AMPLITUDE;
	VECTOR drawPos = VAdd(position, VGet(0.0f, yOffset + BASE_HEIGHT_OFFSET, 0.0f));

	int ringColor = GetColor(255, 200, 0);
	VECTOR center = VGet(position.x, drawPos.y, position.z);

	SetWriteZBuffer3D(FALSE);
	SetDrawBlendMode(DX_BLENDMODE_ADD, 150);

	MATRIX rotYMat = MGetRotY(bobbingTimer);
	MATRIX transMat = MGetTranslate(center);
	MATRIX innerWorldMat = MMult(rotYMat, transMat);

	for (int i = 0; i < RING_SEGMENTS; ++i) {
		float a1 = DX_PI_F * 2.0f * i / RING_SEGMENTS;
		float a2 = DX_PI_F * 2.0f * (i + 1) / RING_SEGMENTS;

		VECTOR localP1 = VGet(std::cosf(a1) * RING_RADIUS_INNER, std::sinf(a1) * RING_RADIUS_INNER, 0.0f);
		VECTOR localP2 = VGet(std::cosf(a2) * RING_RADIUS_INNER, std::sinf(a2) * RING_RADIUS_INNER, 0.0f);

		VECTOR p1 = VTransform(localP1, innerWorldMat);
		VECTOR p2 = VTransform(localP2, innerWorldMat);
		DrawLine3D(p1, p2, ringColor);
	}

	float outerBob = std::sinf(bobbingTimer * 1.5f) * 0.05f;
	MATRIX outerRotMat = MGetRotY(-bobbingTimer * 0.5f);
	MATRIX outerTransMat = MGetTranslate(VAdd(center, VGet(0.0f, -0.2f + outerBob, 0.0f)));
	MATRIX outerWorldMat = MMult(outerRotMat, outerTransMat);

	for (int i = 0; i < RING_SEGMENTS; ++i) {
		float a1 = DX_PI_F * 2.0f * i / RING_SEGMENTS;
		float a2 = DX_PI_F * 2.0f * (i + 1) / RING_SEGMENTS;

		VECTOR localP1 = VGet(std::cosf(a1) * RING_RADIUS_OUTER, 0.0f, std::sinf(a1) * RING_RADIUS_OUTER);
		VECTOR localP2 = VGet(std::cosf(a2) * RING_RADIUS_OUTER, 0.0f, std::sinf(a2) * RING_RADIUS_OUTER);

		VECTOR p1 = VTransform(localP1, outerWorldMat);
		VECTOR p2 = VTransform(localP2, outerWorldMat);
		DrawLine3D(p1, p2, ringColor);
	}

	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	SetWriteZBuffer3D(TRUE);

	int modelHandle = droppedWeapon->GetModelHandle();
	if (modelHandle != -1) {
		const GunStatus& spec = droppedWeapon->GetSpec();

		float s = spec.visual.scale * 0.5;
		MATRIX scaleMat = MGetScale(VGet(s, s, s));

		MATRIX rotMat = MGetRotY(bobbingTimer);
		MATRIX transMat = MGetTranslate(drawPos);

		MATRIX worldMat = MMult(MMult(scaleMat, rotMat), transMat);
		MV1SetMatrix(modelHandle, worldMat);
		MV1DrawModel(modelHandle);
	}
	else {
		DrawCube3D(VAdd(drawPos, VGet(CUBE_HALF_SIZE, CUBE_HALF_SIZE, CUBE_HALF_SIZE)), VAdd(drawPos, VGet(-CUBE_HALF_SIZE, -CUBE_HALF_SIZE, -CUBE_HALF_SIZE)), GetColor(255, 255, 0), GetColor(255, 255, 0), TRUE);
	}

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

			
				float fadeStartDist = UI_DISPLAY_DISTANCE - 5.0f;
				float alphaRate = 1.0f;
				if (distance > fadeStartDist) {
					alphaRate = 1.0f - ((distance - fadeStartDist) / 5.0f);
				}
				int alpha = static_cast<int>(255 * alphaRate);

				::SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
				// ★修正: フォント指定＋白文字に変更
				::DrawFormatStringToHandle(drawX, drawYAmmo, GetColor(255, 255, 255), fontItem, "AMMO: %d / %d", droppedWeapon->GetAmmo(), droppedWeapon->GetReserveAmmo());
				::DrawFormatStringToHandle(drawX, drawYName, GetColor(255, 255, 255), fontItem, "%s", weaponName);
				::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			}
		}
	}
}