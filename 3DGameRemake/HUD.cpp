#include "HUD.h"
#include "Player.h"
#include "Weapon.h"
#include "Parameter.h"
#include "Time.h"
#include "TextManager.h"
#include "ItemManager.h"
#include "EnemyManager.h"
#include "GameScene.h"
#include "ResourceManager.h"
#include "SoundManager.h"

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

HUD::HUD(Player* player) : pplayer(player){
	fontJpLarge = ResourceManager::GetIns().GetFont("Resource/Font/NotoSansJP_32.dft");
	fontJpMedium = ResourceManager::GetIns().GetFont("Resource/Font/NotoSansJP_20.dft");
	fontEnLarge = ResourceManager::GetIns().GetFont("Resource/Font/RobotoMono_28.dft");
	fontEnSmall = ResourceManager::GetIns().GetFont("Resource/Font/RobotoMono_20.dft");
}

void HUD::Update() {
	float dt = Time::GetIns().GetDelta();

	if (damageFlashTimer > 0.0f) {
		damageFlashTimer -= dt;
	}

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
	int textColor = GetColor(255, 255, 255);
	int cenX = CENTER_X;
	int topY = 50;
	char topTextBuf[128];
	if (prepareTime > 0.0f) {
		snprintf(topTextBuf, sizeof(topTextBuf), "スタートまで : %.1f", prepareTime);
	}
	else {
		snprintf(topTextBuf, sizeof(topTextBuf), "スコア : %d", EnemyManager::GetIns().GetTotalScore());
	}

	int textWidth = GetDrawStringWidthToHandle(topTextBuf, static_cast<int>(strlen(topTextBuf)), fontJpLarge);
	int drawX = CENTER_X - (textWidth / 2);

	if (prepareTime > 0.0f) {
		::DrawStringToHandle(drawX, topY, topTextBuf, textColor, fontJpLarge);
	}
	else {
		int alpha = static_cast<int>(scoreFadeAlpha * 255);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
		::DrawStringToHandle(drawX, topY, topTextBuf, textColor, fontJpLarge);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}

	if (hitMarkTimer > 0.0f) {
		const float HITMARK_DURATION = 0.2f;
		float progress = 1.0f - (hitMarkTimer / HITMARK_DURATION);
		float easeOut = 1.0f - std::powf(1.0f - progress, 3.0f);

		int cx = CENTER_X;
		int cy = CENTER_Y;

		// 3段階の状態に応じたパラメータ設定
		int baseColor = GetColor(255, 255, 255); // 通常: 白
		int thickness = 2;
		float lineLength = 8.0f;
		float offsetMultiplier = 12.0f;

		if (lastHitWasKill) {
			baseColor = GetColor(255, 50, 50); // キル: 赤
			thickness = 4;
			lineLength = 18.0f;       // キル時は線を一番長くする
			offsetMultiplier = 24.0f; // キル時は一番大きく弾ける
		}
		else if (lastHitWasHS) {
			baseColor = GetColor(255, 200, 0); // ヘッドショット: 黄色
			thickness = 4;
			lineLength = 14.0f;
			offsetMultiplier = 20.0f;
		}

		int alpha = static_cast<int>(255 * (1.0f - easeOut));
		float baseOffset = 8.0f;
		float expandOffset = baseOffset + offsetMultiplier * easeOut;

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);

		DrawLine(cx - static_cast<int>(expandOffset + lineLength), cy - static_cast<int>(expandOffset + lineLength),
			cx - static_cast<int>(expandOffset), cy - static_cast<int>(expandOffset), baseColor, thickness);
		DrawLine(cx + static_cast<int>(expandOffset + lineLength), cy - static_cast<int>(expandOffset + lineLength),
			cx + static_cast<int>(expandOffset), cy - static_cast<int>(expandOffset), baseColor, thickness);
		DrawLine(cx - static_cast<int>(expandOffset + lineLength), cy + static_cast<int>(expandOffset + lineLength),
			cx - static_cast<int>(expandOffset), cy + static_cast<int>(expandOffset), baseColor, thickness);
		DrawLine(cx + static_cast<int>(expandOffset + lineLength), cy + static_cast<int>(expandOffset + lineLength),
			cx + static_cast<int>(expandOffset), cy + static_cast<int>(expandOffset), baseColor, thickness);

		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}

	if (damageFlashTimer > 0.0f) {
		float progress = 1.0f - (damageFlashTimer / DAMAGE_FLASH_DURATION);

		float ease = 1.0f - std::powf(1.0f - progress, 4.0f);

		int alpha = static_cast<int>(180 * (1.0f - ease));
		float baseThickness = 40.0f;
		float thickness = baseThickness * (1.0f - ease);
		float offset = 60.0f * ease;

		int color = GetColor(255, 0, 0);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);

		int t = static_cast<int>(thickness);
		int o = static_cast<int>(offset);

		DrawBox(0, -o, WINDOW_WIDTH, t - o, color, TRUE);
		DrawBox(0, WINDOW_HEIGHT - t + o, WINDOW_WIDTH, WINDOW_HEIGHT + o, color, TRUE);
		DrawBox(-o, t - o, t - o, WINDOW_HEIGHT - t + o, color, TRUE); // 左
		DrawBox(WINDOW_WIDTH - t + o, t - o, WINDOW_WIDTH + o, WINDOW_HEIGHT - t + o, color, TRUE); // 右

		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
	WeaponItem* nearItem = ItemManager::GetIns().GetNearItem();
	if (nearItem) {
		const GunStatus* spec = nearItem->GetSpec();
		if (spec) {
			const char* weaponName = TextManager::GetIns().GetWeaponName(spec->id);

			std::string interactKey = TextManager::GetIns().GetActionKeyString(ActionID::INTERACT);
			std::string pickUpStr = "PickUp : " + interactKey;

			int nameWidth = GetDrawStringWidthToHandle(weaponName, static_cast<int>(strlen(weaponName)), fontJpMedium);
			int subTextWidth = GetDrawStringWidthToHandle(pickUpStr.c_str(), static_cast<int>(pickUpStr.length()), fontEnSmall);
			int textMaxWidth = (nameWidth > subTextWidth) ? nameWidth : subTextWidth;

			int iconSize = spec->visual.uiPath.empty() ? 0 : 100;
			int gap = (iconSize > 0) ? 20 : 0;

			int paddingX = 25;
			int paddingY = 15;
			int contentWidth = iconSize + gap + textMaxWidth;
			int boxWidth = contentWidth + paddingX * 2;
			int boxHeight = (iconSize > 0) ? iconSize + paddingY * 2 : 70;

	
			int boxStartX = CENTER_X - (boxWidth / 2);
			int boxStartY = CENTER_Y + 120;


			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 150);
			DrawBox(boxStartX, boxStartY, boxStartX + boxWidth, boxStartY + boxHeight, GetColor(0, 0, 0), TRUE);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

			int currentX = boxStartX + paddingX;
			int white = GetColor(255, 255, 255);


			if (iconSize > 0) {
				int iconHandle = ResourceManager::GetIns().GetGraph(spec->visual.uiPath);
				if (iconHandle != -1) {
					SetDrawBright(255, 255, 255);
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, 200);
					DrawExtendGraph(currentX, boxStartY + paddingY, currentX + iconSize, boxStartY + paddingY + iconSize, iconHandle, TRUE);
					SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
				}
				currentX += iconSize + gap; 
			}


			int textStartY = boxStartY + paddingY + (iconSize / 2) - 25;
			if (iconSize == 0) textStartY = boxStartY + 10;

			::DrawStringToHandle(currentX, textStartY, weaponName, white, fontJpMedium);
			::DrawStringToHandle(currentX, textStartY + 30, pickUpStr.c_str(), white, fontEnSmall);
		}
	}

	int hp = pplayer->GetHP();
	int maxHP = pplayer->GetStatus().maxHP;
	float hpRatio = static_cast<float>(hp) / maxHP;
	if (hpRatio < 0.0f) hpRatio = 0.0f;

	int hpBarWidth = 600;
	int hpBarHeight = 10;
	int hpStartX = CENTER_X - (hpBarWidth / 2);
	int hpStartY = WINDOW_HEIGHT - 30;


	DrawBox(hpStartX, hpStartY, hpStartX + hpBarWidth, hpStartY + hpBarHeight, GetColor(40, 40, 40), TRUE);

	int hpColor = (hpRatio > 0.3f) ? GetColor(255, 255, 255) : GetColor(255, 50, 50);
	DrawBox(hpStartX, hpStartY, hpStartX + static_cast<int>(hpBarWidth * hpRatio), hpStartY + hpBarHeight, hpColor, TRUE);

	Weapon* weapon = pplayer->GetWeapon();

	if (weapon) {
		const GunStatus& spec = weapon->GetSpec();

		if (!weapon->TakingAim() || spec.id == WeaponID::LR || spec.id == WeaponID::SMG) {
			DrawCircle(CENTER_X, CENTER_Y, CROSSHAIR_DOT_SIZE, GetColor(255, 255, 255), true);
		}
		int ammo = weapon->GetAmmo();
		int mag = spec.magAmmo;
		int reserve = weapon->GetReserveAmmo();
		const char* weaponName = TextManager::GetIns().GetWeaponName(spec.id);

		int iconSize = 250;
		int uiBaseX = WINDOW_WIDTH - iconSize - 30;
		int uiBaseY = WINDOW_HEIGHT - iconSize - 20;

		if (!spec.visual.uiPath.empty()) {
			int iconHandle = ResourceManager::GetIns().GetGraph(spec.visual.uiPath);
			if (iconHandle != -1) {
				SetDrawBright(0, 0, 0);
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, 150);
				DrawExtendGraph(uiBaseX, uiBaseY, uiBaseX + iconSize, uiBaseY + iconSize, iconHandle, TRUE);
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
				SetDrawBright(255, 255, 255);
			}
		}

		int textX = uiBaseX + 20;
		int textY = uiBaseY + iconSize - 100;

		::DrawFormatStringToHandle(textX, textY, GetColor(255, 255, 255), fontJpMedium, "%s", weaponName);
		::DrawFormatStringToHandle(textX, textY + 30, GetColor(255, 255, 255), fontEnLarge, "AMMO  %02d / %02d", ammo, mag);

		if (!weapon->IsInfinite()) {
			::DrawFormatStringToHandle(textX, textY + 60, GetColor(200, 200, 200), fontEnSmall, "      %03d", reserve);
		}

		if (weapon->Reloading()) {
			float prog = weapon->GetReloadProgress();
			int cx = CENTER_X;
			int cy = CENTER_Y;
			int radius = 128;
			int segments = 128;

			DrawCircle(cx, cy, radius, GetColor(80, 80, 80), FALSE);

			int arcColor = GetColor(255, 200, 0);
			for (int i = 0; i < static_cast<int>(segments * prog); ++i) {
				float a1 = -DX_PI_F / 2.0f + (DX_PI_F * 2.0f * i / segments);
				float a2 = -DX_PI_F / 2.0f + (DX_PI_F * 2.0f * (i + 1) / segments);

				int x1 = cx + static_cast<int>(cosf(a1) * radius);
				int y1 = cy + static_cast<int>(sinf(a1) * radius);
				int x2 = cx + static_cast<int>(cosf(a2) * radius);
				int y2 = cy + static_cast<int>(sinf(a2) * radius);

				DrawLine(x1, y1, x2, y2, arcColor, 3);
			}

			const char* reloadText = "RELOADING";
			int rWidth = GetDrawStringWidthToHandle(reloadText, static_cast<int>(strlen(reloadText)), fontEnSmall);
			::DrawStringToHandle(cx - (rWidth / 2), cy + radius + 15, reloadText, arcColor, fontEnSmall);
		}

		const auto& attackers = pplayer->GetTargeted();
		if (!attackers.empty()) {
			float playerYaw = pplayer->GetCam()->GetYaw() * (DX_PI_F / 180.0f);

			for (const auto& info : attackers) {
				VECTOR toEnemy = VSub(info.pos, pplayer->GetPos());
				float enemyAngle = atan2f(toEnemy.x, toEnemy.z);
				float relativeAngle = enemyAngle - playerYaw;

				float prog = info.progress;

				float radius = INDICATOR_RADIUS + 80.0f * (1.0f - prog);
				float arcAngle = (DX_PI_F / 4.0f) * (1.0f - prog) + (DX_PI_F / 16.0f);

				int color = GetColor(255, static_cast<int>(255 * (1.0f - prog)), 0);
				int alpha = static_cast<int>(100 + 155 * prog);

				int cx = CENTER_X;
				int cy = CENTER_Y;
				int segments = 16;

				SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
				for (int i = 0; i < segments; ++i) {
					float a1 = relativeAngle - arcAngle + (arcAngle * 2.0f * i / segments);
					float a2 = relativeAngle - arcAngle + (arcAngle * 2.0f * (i + 1) / segments);

					int x1 = cx + static_cast<int>(sinf(a1) * radius);
					int y1 = cy - static_cast<int>(cosf(a1) * radius);
					int x2 = cx + static_cast<int>(sinf(a2) * radius);
					int y2 = cy - static_cast<int>(cosf(a2) * radius);

					DrawLine(x1, y1, x2, y2, color, 3);
				}
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			}
		}
	}

}

void HUD::OnHitTarget(bool isHeadShot,bool isKill) {
	hitMarkTimer = HITMARK_DURATION;
	lastHitWasHS = isHeadShot;
	lastHitWasKill = isKill;
	if (isKill) {
		SoundManager::GetIns().PlaySE("Resource/Sound/kill.ogg");
	}
	else if (isHeadShot) {
		SoundManager::GetIns().PlaySE("Resource/Sound/hs.ogg");
	}
	else {
		SoundManager::GetIns().PlaySE("Resource/Sound/hit.ogg");
	}
}

void HUD::OnPlayerTakeDamage() {
	damageFlashTimer = DAMAGE_FLASH_DURATION;
	if (pplayer->GetHP() > 0) {
		SoundManager::GetIns().PlaySE("Resource/Sound/damage.ogg");
	}
}