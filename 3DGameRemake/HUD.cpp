#include "HUD.h"
#include "Player.h"
#include "Weapon.h"
#include "Parameter.h"
#include "Time.h"
#include "TextManager.h"

HUD::HUD(Player* player) : pplayer(player),cancelReloadTimer(0.0f) {}

void HUD::Update() {
	if (cancelReloadTimer >= 0) {
		cancelReloadTimer -= Time::GetIns().GetDelta();
		if (cancelReloadTimer < 0) {
			cancelReloadTimer = 0;
		}
	}
}

void HUD::Draw() {
	if (pplayer) {
		int dotSize = 4;
		DrawCircle(CENTER_X, CENTER_Y, dotSize, GetColor(0, 0, 0), true);

		int hp = pplayer->GetHP();
		int maxHP = pplayer->GetStatus().maxHP;

		DrawFormatString(50, WINDOW_HEIGHT - 80, GetColor(0, 0, 0), "HP:%d / %d", hp, maxHP);

		Weapon* weapon = pplayer->GetWeapon();

		if (weapon) {
			int ammo = weapon->GetAmmo();
			int mag = weapon->GetSpec().magAmmo;
			int reserve = weapon->GetReserveAmmo();
			const char* weaponName = TextManager::GetIns().GetWeaponName(pplayer->GetWeapon()->GetSpec().id);
			DrawFormatString(WINDOW_WIDTH - 200, WINDOW_HEIGHT - 110, GetColor(0, 0, 0), "%s", weaponName);
			DrawFormatString(WINDOW_WIDTH - 200, WINDOW_HEIGHT - 80, GetColor(0, 0, 0), "AMMO: %d / %d", ammo, mag);

			if (!weapon->IsInfinite()) {
				DrawFormatString(WINDOW_WIDTH - 200, WINDOW_HEIGHT - 50, GetColor(0, 0, 0), "RESERVE: %d", reserve);
			}

			if (weapon->Reloading()) {
				DrawString(CENTER_X - 40, CENTER_Y + 40, "RELOADING", GetColor(0, 0, 0));
			}

	/* 
			if (weapon->IsReloadCanceled() || cancelReloadTimer > 0) {
				DrawString(CENTER_X - 40, CENTER_Y + 60, "CANCELED", GetColor(0, 0, 0));
				weapon->SetReloadCanceled();
				cancelReloadTimer = 1.5f;
			}
	*/
			const auto& attackers = pplayer->GetTargeted();
			if (attackers.empty())return;

			float playerYaw = pplayer->GetCam()->GetYaw() * (DX_PI_F / 180.0f);
			float radius = 100.0f;

			for (const auto& enemyPos : attackers) {
				VECTOR screenPos = ConvWorldPosToScreenPos(enemyPos);

				if (screenPos.z >= 0.0f && screenPos.z <= 1.0f &&
					screenPos.x >= 0 && screenPos.x <= WINDOW_WIDTH &&
					screenPos.y >= 0 && screenPos.y <= WINDOW_HEIGHT) continue;
				VECTOR toEnemy = VSub(enemyPos, pplayer->GetPos());

				float enemyAngle = atan2f(toEnemy.x, toEnemy.z);

				float relativeAngle = enemyAngle - playerYaw;

				int drawX = CENTER_X + (int)(sinf(relativeAngle) * radius);
				int drawY = CENTER_Y - (int)(cosf(relativeAngle) * radius);

				DrawCircle(drawX, drawY, 5, GetColor(255, 0, 0),TRUE);
			}
		}
	}
}