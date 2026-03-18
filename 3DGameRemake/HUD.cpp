#include "HUD.h"
#include "Player.h"
#include "Weapon.h"
#include "Parameter.h"
#include "Time.h"
#include "TextManager.h"
#include "ItemManager.h"
#include "EnemyManager.h"
#include "GameScene.h"
#include <cmath>

namespace {
	constexpr int CROSSHAIR_DOT_SIZE = 4;

	constexpr float HITMARK_DURATION = 0.2f;
	constexpr int HITMARK_OFFSET = 8;
	constexpr int HITMARK_LENGTH = 8;
	constexpr int HITMARK_THICKNESS = 3;

	constexpr int PICKUP_UI_OFFSET_X = -120;
	constexpr int PICKUP_UI_OFFSET_Y = 120;

	constexpr int HP_UI_X = 50;
	constexpr int HP_UI_OFFSET_Y = -80;

	constexpr int WEAPON_UI_OFFSET_X = -200;
	constexpr int WEAPON_NAME_OFFSET_Y = -110;
	constexpr int AMMO_UI_OFFSET_Y = -80;
	constexpr int RESERVE_UI_OFFSET_Y = -50;

	constexpr int RELOAD_UI_OFFSET_X = -40;
	constexpr int RELOAD_UI_OFFSET_Y = 40;

	constexpr float INDICATOR_RADIUS = 100;
	constexpr int INDICATOR_DOT_SIZE = 5;

	constexpr float FADE_DURATION = 0.3f;
}

HUD::HUD(Player* player) : pplayer(player){}

void HUD::Update() {
	float dt = Time::GetIns().GetDelta();

	if (hitMarkTimer > 0.0f) {
		hitMarkTimer -= dt;
	}

	float prepareTime = EnemyManager::GetIns().GetPrepareTimer();
	if (prepareTime <= 0.0f && !isGameStarted) {
		isGameStarted = true;
		scoreFadeAlpha = 0.0f;
	}

	if (isGameStarted && scoreFadeAlpha < 1.0f) {
		scoreFadeAlpha += dt / FADE_DURATION;
		if (scoreFadeAlpha > 1.0f)scoreFadeAlpha = 1.0f;
	}
}

void HUD::Draw() {
	if (!pplayer)return;

	float prepareTime = EnemyManager::GetIns().GetPrepareTimer();
	int textColor = GetColor(0, 0, 0);
	int cenX = CENTER_X;
	int topY = 50;
	if (prepareTime > 0.0f) {
		DrawFormatString(cenX - 80, topY, textColor, "スタートまで : %.1f", prepareTime);
	}
	else {
		int alpha = static_cast<int>(scoreFadeAlpha * 255);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
		DrawFormatString(cenX - 40, topY, textColor, "スコア : %6d", EnemyManager::GetIns().GetTotalScore());
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
		DrawCircle(CENTER_X, CENTER_Y, CROSSHAIR_DOT_SIZE, GetColor(0, 0, 0), true);

	if (hitMarkTimer > 0.0f) {
		int cx = CENTER_X;
		int cy = CENTER_Y;

		int color = lastHitWasHS ? GetColor(255, 0, 0) : GetColor(0, 0, 0);

		DrawLine(cx - HITMARK_OFFSET - HITMARK_LENGTH, cy - HITMARK_OFFSET - HITMARK_LENGTH, cx - HITMARK_OFFSET, cy - HITMARK_OFFSET, color, 3);
		DrawLine(cx + HITMARK_OFFSET + HITMARK_LENGTH, cy - HITMARK_OFFSET - HITMARK_LENGTH, cx + HITMARK_OFFSET, cy - HITMARK_OFFSET, color, 3);
		DrawLine(cx - HITMARK_OFFSET - HITMARK_LENGTH, cy + HITMARK_OFFSET + HITMARK_LENGTH, cx - HITMARK_OFFSET, cy + HITMARK_OFFSET, color, 3);
		DrawLine(cx + HITMARK_OFFSET + HITMARK_LENGTH, cy + HITMARK_OFFSET + HITMARK_LENGTH, cx + HITMARK_OFFSET, cy + HITMARK_OFFSET, color, 3);
	}

	WeaponItem* nearItem = ItemManager::GetIns().GetNearItem();
	if (nearItem) {
		const GunStatus* spec = nearItem->GetSpec();
		if (spec) {
			const char* weaponName = TextManager::GetIns().GetWeaponName(spec->id);

			DrawFormatString(CENTER_X + PICKUP_UI_OFFSET_X, CENTER_Y + PICKUP_UI_OFFSET_Y, GetColor(0, 0, 0), "PickUp : %s", weaponName);
		}
	}
	int hp = pplayer->GetHP();
	int maxHP = pplayer->GetStatus().maxHP;

	DrawFormatString(HP_UI_X, WINDOW_HEIGHT + HP_UI_OFFSET_Y, GetColor(0, 0, 0), "HP:%d / %d", hp, maxHP);

	Weapon* weapon = pplayer->GetWeapon();

	if (weapon) {
		int ammo = weapon->GetAmmo();
		int mag = weapon->GetSpec().magAmmo;
		int reserve = weapon->GetReserveAmmo();
		const char* weaponName = TextManager::GetIns().GetWeaponName(pplayer->GetWeapon()->GetSpec().id);
		int weaponUIX = WINDOW_WIDTH + WEAPON_UI_OFFSET_X;
		DrawFormatString(weaponUIX, WINDOW_HEIGHT + WEAPON_NAME_OFFSET_Y, GetColor(0, 0, 0), "%s", weaponName);
		DrawFormatString(weaponUIX, WINDOW_HEIGHT + AMMO_UI_OFFSET_Y, GetColor(0, 0, 0), "AMMO: %d / %d", ammo, mag);

		if (!weapon->IsInfinite()) {
			DrawFormatString(weaponUIX, WINDOW_HEIGHT + RESERVE_UI_OFFSET_Y, GetColor(0, 0, 0), "RESERVE: %d", reserve);
		}

		if (weapon->Reloading()) {
			DrawString(CENTER_X + RELOAD_UI_OFFSET_X, CENTER_Y + RELOAD_UI_OFFSET_Y, "RELOADING", GetColor(0, 0, 0));
		}

		const auto& attackers = pplayer->GetTargeted();
		if (attackers.empty())return;

		float playerYaw = pplayer->GetCam()->GetYaw() * (DX_PI_F / 180.0f);


		for (const auto& enemyPos : attackers) {
			VECTOR screenPos = ConvWorldPosToScreenPos(enemyPos);

			if (screenPos.z >= 0.0f && screenPos.z <= 1.0f &&
				screenPos.x >= 0 && screenPos.x <= WINDOW_WIDTH &&
				screenPos.y >= 0 && screenPos.y <= WINDOW_HEIGHT) continue;
			VECTOR toEnemy = VSub(enemyPos, pplayer->GetPos());

			float enemyAngle = atan2f(toEnemy.x, toEnemy.z);

			float relativeAngle = enemyAngle - playerYaw;

			int drawX = CENTER_X + static_cast<int>(sinf(relativeAngle) * INDICATOR_RADIUS);
			int drawY = CENTER_Y - static_cast<int>(cosf(relativeAngle) * INDICATOR_RADIUS);

			DrawCircle(drawX, drawY, INDICATOR_DOT_SIZE, GetColor(255, 0, 0), TRUE);
		}
	}

}

void HUD::OnHitTarget(bool isHeadShot) {
	hitMarkTimer = HITMARK_DURATION;
	lastHitWasHS = isHeadShot;
}