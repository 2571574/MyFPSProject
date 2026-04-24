#include "WeaponItem.h"
#include "Time.h"
#include "TextManager.h"
#include "ItemManager.h"
#include "ResourceManager.h"
#include "Param/Global.h"
#include "Param/System.h"
#include "Param/Visual.h"

#include <cmath>

WeaponItem::WeaponItem(VECTOR pos, std::unique_ptr<Weapon>weapon)
	:position(pos)
	, droppedWeapon(std::move(weapon))
	, bobbingTimer(0.0f)
	, alive(true) {
	fontItemAmmo = ResourceManager::GetIns().GetFont("Resource/Font/RobotoMono_20.dft");
	fontItemName = ResourceManager::GetIns().GetFont("Resource/Font/NotoSansJP_20.dft");
}

void WeaponItem::Update() {
	float dt = Time::GetIns().GetDelta();
	bobbingTimer += dt * Visual::ItemUI::BOBBING_SPEED;
}

void WeaponItem::Draw() {
	if (!alive || !droppedWeapon)return;

	float yOffset = std::sinf(bobbingTimer) * Visual::ItemUI::BOBBING_AMPLITUDE;
	VECTOR drawPos = VAdd(position, VGet(0.0f, yOffset + Visual::ItemUI::BASE_HEIGHT_OFFSET, 0.0f));

	int ringColor = GetColor(Visual::ItemUI::COLOR_ITEM_RING.r, Visual::ItemUI::COLOR_ITEM_RING.g, Visual::ItemUI::COLOR_ITEM_RING.b);
	VECTOR center = VGet(position.x, drawPos.y, position.z);

	SetWriteZBuffer3D(FALSE);
	SetDrawBlendMode(DX_BLENDMODE_ADD, Visual::ItemUI::RING_BLEND_ALPHA);

	MATRIX rotYMat = MGetRotY(bobbingTimer);
	MATRIX transMat = MGetTranslate(center);
	MATRIX innerWorldMat = MMult(rotYMat, transMat);

	for (int i = 0; i < Visual::ItemUI::RING_SEGMENTS; ++i) {
		float a1 = DX_PI_F * 2.0f * i / Visual::ItemUI::RING_SEGMENTS;
		float a2 = DX_PI_F * 2.0f * (i + 1) / Visual::ItemUI::RING_SEGMENTS;

		VECTOR localP1 = VGet(std::cosf(a1) * Visual::ItemUI::RING_RADIUS_INNER, std::sinf(a1) * Visual::ItemUI::RING_RADIUS_INNER, 0.0f);
		VECTOR localP2 = VGet(std::cosf(a2) * Visual::ItemUI::RING_RADIUS_INNER, std::sinf(a2) * Visual::ItemUI::RING_RADIUS_INNER, 0.0f);

		VECTOR p1 = VTransform(localP1, innerWorldMat);
		VECTOR p2 = VTransform(localP2, innerWorldMat);
		DrawLine3D(p1, p2, ringColor);
	}

	float outerBob = std::sinf(bobbingTimer * Visual::ItemUI::OUTER_RING_BOB_SPEED_MULT) * Visual::ItemUI::OUTER_RING_BOB_AMP;
	MATRIX outerRotMat = MGetRotY(bobbingTimer * Visual::ItemUI::OUTER_RING_ROT_SPEED_MULT);
	MATRIX outerTransMat = MGetTranslate(VAdd(center, VGet(0.0f, Visual::ItemUI::OUTER_RING_OFFSET_Y + outerBob, 0.0f)));
	MATRIX outerWorldMat = MMult(outerRotMat, outerTransMat);

	for (int i = 0; i < Visual::ItemUI::RING_SEGMENTS; ++i) {
		float a1 = DX_PI_F * 2.0f * i / Visual::ItemUI::RING_SEGMENTS;
		float a2 = DX_PI_F * 2.0f * (i + 1) / Visual::ItemUI::RING_SEGMENTS;

		VECTOR localP1 = VGet(std::cosf(a1) * Visual::ItemUI::RING_RADIUS_OUTER, 0.0f, std::sinf(a1) * Visual::ItemUI::RING_RADIUS_OUTER);
		VECTOR localP2 = VGet(std::cosf(a2) * Visual::ItemUI::RING_RADIUS_OUTER, 0.0f, std::sinf(a2) * Visual::ItemUI::RING_RADIUS_OUTER);

		VECTOR p1 = VTransform(localP1, outerWorldMat);
		VECTOR p2 = VTransform(localP2, outerWorldMat);
		DrawLine3D(p1, p2, ringColor);
	}

	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	SetWriteZBuffer3D(TRUE);

	int modelHandle = droppedWeapon->GetModelHandle();
	if (modelHandle != -1) {
		const GunStatus& spec = droppedWeapon->GetSpec();

		float s = spec.visual.scale * 0.5f;
		MATRIX scaleMat = MGetScale(VGet(s, s, s));
		MATRIX rotMat = MGetRotY(bobbingTimer);
		MATRIX transMat = MGetTranslate(drawPos);

		MATRIX worldMat = MMult(MMult(scaleMat, rotMat), transMat);
		MV1SetMatrix(modelHandle, worldMat);
		MV1DrawModel(modelHandle);
	}
	else {
		int fallbackColor = GetColor(Visual::ItemUI::COLOR_FALLBACK_CUBE.r, Visual::ItemUI::COLOR_FALLBACK_CUBE.g, Visual::ItemUI::COLOR_FALLBACK_CUBE.b);
		DrawCube3D(VAdd(drawPos, VGet(Visual::ItemUI::CUBE_HALF_SIZE, Visual::ItemUI::CUBE_HALF_SIZE, Visual::ItemUI::CUBE_HALF_SIZE)),
			VAdd(drawPos, VGet(-Visual::ItemUI::CUBE_HALF_SIZE, -Visual::ItemUI::CUBE_HALF_SIZE, -Visual::ItemUI::CUBE_HALF_SIZE)),
			fallbackColor, fallbackColor, TRUE);
	}

	VECTOR camPos = ItemManager::GetIns().GetCamPos();
	float distance = VSize(VSub(camPos, drawPos));
	if (distance <= Visual::ItemUI::UI_DISPLAY_DISTANCE) {
		VECTOR screenPos = ConvWorldPosToScreenPos(drawPos);
		if (screenPos.z >= 0.0f && screenPos.z <= 1.0f) {
			int stageHandle = ItemManager::GetIns().GetStageHandle();
			MV1_COLL_RESULT_POLY hit = MV1CollCheck_Line(stageHandle, -1, camPos, drawPos);
			if (hit.HitFlag == 0) {
				const char* weaponName = TextManager::GetIns().GetWeaponName(droppedWeapon->GetSpec().id);

				int drawX = static_cast<int>(screenPos.x) + Visual::ItemUI::UI_OFFSET_X;
				int drawYAmmo = static_cast<int>(screenPos.y) + Visual::ItemUI::UI_OFFSET_Y_AMMO;
				int drawYName = static_cast<int>(screenPos.y) + Visual::ItemUI::UI_OFFSET_Y_NAME;

				float fadeStartDist = Visual::ItemUI::UI_DISPLAY_DISTANCE - Visual::ItemUI::UI_FADE_TRANSITION_RANGE;
				float alphaRate = 1.0f;
				if (distance > fadeStartDist) {
					alphaRate = 1.0f - ((distance - fadeStartDist) / Visual::ItemUI::UI_FADE_TRANSITION_RANGE);
				}
				int alpha = static_cast<int>(255 * alphaRate);

				::SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
				::DrawFormatStringToHandle(drawX, drawYAmmo, GetColor(Visual::HUD::COLOR_HUD_WHITE.r, Visual::HUD::COLOR_HUD_WHITE.g, Visual::HUD::COLOR_HUD_WHITE.b), fontItemAmmo, "AMMO: %d / %d", droppedWeapon->GetAmmo(), droppedWeapon->GetReserveAmmo());
				::DrawFormatStringToHandle(drawX, drawYName, GetColor(Visual::HUD::COLOR_HUD_WHITE.r, Visual::HUD::COLOR_HUD_WHITE.g, Visual::HUD::COLOR_HUD_WHITE.b), fontItemName, "%s", weaponName);
				::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			}
		}
	}
}