#include "HUD.h"
#include "Player.h"
#include "Weapon.h"
#include "Time.h"
#include "TextManager.h"
#include "ItemManager.h"
#include "EnemyManager.h"
#include "GameScene.h"
#include "ResourceManager.h"
#include "SoundManager.h"
#include "Param/Global.h"
#include "Param/System.h"
#include "Param/Visual.h"

#include <cmath>

HUD::HUD(Player* player) : pplayer(player) {
	fontJpLarge = ResourceManager::GetIns().GetFont("Resource/Font/NotoSansJP_32.dft");
	fontJpMedium = ResourceManager::GetIns().GetFont("Resource/Font/NotoSansJP_20.dft");
	fontEnLarge = ResourceManager::GetIns().GetFont("Resource/Font/RobotoMono_28.dft");
	fontEnSmall = ResourceManager::GetIns().GetFont("Resource/Font/RobotoMono_20.dft");
}

void HUD::Update() {
	float dt = Time::GetIns().GetDelta();

	if (damageFlashTimer > 0.0f) damageFlashTimer -= dt;
	if (hitMarkTimer > 0.0f) hitMarkTimer -= dt;

	float prepareTime = EnemyManager::GetIns().GetPrepareTimer();
	if (prepareTime <= 0.0f && !isGameStarted) {
		isGameStarted = true;
		scoreFadeAlpha = 0.0f;
	}

	if (isGameStarted && scoreFadeAlpha < 1.0f) {
		scoreFadeAlpha += dt / Visual::HUD::FADE_DURATION;
		if (scoreFadeAlpha > 1.0f) scoreFadeAlpha = 1.0f;
	}
}

void HUD::Draw() {
	if (!pplayer)return;

	float prepareTime = EnemyManager::GetIns().GetPrepareTimer();
	int textColor = GetColor(Visual::HUD::COLOR_HUD_WHITE.r, Visual::HUD::COLOR_HUD_WHITE.g, Visual::HUD::COLOR_HUD_WHITE.b);
	int topY = Visual::HUD::TOP_INFO_TEXT_Y;
	char topTextBuf[128];
	if (prepareTime > 0.0f) {
		snprintf(topTextBuf, sizeof(topTextBuf), "スタートまで : %.1f", prepareTime);
	}
	else {
		snprintf(topTextBuf, sizeof(topTextBuf), "スコア : %d", EnemyManager::GetIns().GetTotalScore());
	}

	int textWidth = GetDrawStringWidthToHandle(topTextBuf, static_cast<int>(strlen(topTextBuf)), fontJpLarge);
	int drawX = System::Window::CENTER_X - (textWidth / 2);

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
		float progress = 1.0f - (hitMarkTimer / Visual::HUD::HITMARK_DURATION);
		float easeOut = 1.0f - std::powf(1.0f - progress, Visual::HUD::HITMARK_EASE_OUT_POWER);

		int cx = System::Window::CENTER_X;
		int cy = System::Window::CENTER_Y;

		Global::ColorRGB bColor = Visual::HUD::COLOR_HUD_WHITE;
		int thickness = Visual::HUD::HITMARK_THICKNESS_NORMAL;
		float lineLength = Visual::HUD::HITMARK_LINE_LENGTH_NORMAL;
		float offsetMultiplier = Visual::HUD::HITMARK_OFFSET_MULTIPLIER_NORMAL;

		if (lastHitWasKill) {
			bColor = Visual::HUD::COLOR_HUD_RED;
			thickness = Visual::HUD::HITMARK_THICKNESS_KILL;
			lineLength = Visual::HUD::HITMARK_LINE_LENGTH_KILL;
			offsetMultiplier = Visual::HUD::HITMARK_OFFSET_MULTIPLIER_KILL;
		}
		else if (lastHitWasHS) {
			bColor = Visual::HUD::COLOR_HUD_YELLOW;
			thickness = Visual::HUD::HITMARK_THICKNESS_HS;
			lineLength = Visual::HUD::HITMARK_LINE_LENGTH_HS;
			offsetMultiplier = Visual::HUD::HITMARK_OFFSET_MULTIPLIER_HS;
		}

		int baseColor = GetColor(bColor.r, bColor.g, bColor.b);
		int alpha = static_cast<int>(255 * (1.0f - easeOut));
		float expandOffset = Visual::HUD::HITMARK_BASE_OFFSET + offsetMultiplier * easeOut;

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);

		DrawLine(cx - static_cast<int>(expandOffset + lineLength), cy - static_cast<int>(expandOffset + lineLength), cx - static_cast<int>(expandOffset), cy - static_cast<int>(expandOffset), baseColor, thickness);
		DrawLine(cx + static_cast<int>(expandOffset + lineLength), cy - static_cast<int>(expandOffset + lineLength), cx + static_cast<int>(expandOffset), cy - static_cast<int>(expandOffset), baseColor, thickness);
		DrawLine(cx - static_cast<int>(expandOffset + lineLength), cy + static_cast<int>(expandOffset + lineLength), cx - static_cast<int>(expandOffset), cy + static_cast<int>(expandOffset), baseColor, thickness);
		DrawLine(cx + static_cast<int>(expandOffset + lineLength), cy + static_cast<int>(expandOffset + lineLength), cx + static_cast<int>(expandOffset), cy + static_cast<int>(expandOffset), baseColor, thickness);

		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}

	if (damageFlashTimer > 0.0f) {
		float progress = 1.0f - (damageFlashTimer / Visual::HUD::DAMAGE_FLASH_DURATION);
		float ease = 1.0f - std::powf(1.0f - progress, Visual::HUD::DAMAGE_FLASH_EASE_POWER);

		int alpha = static_cast<int>(Visual::HUD::DAMAGE_FLASH_ALPHA_MAX * (1.0f - ease));
		float thickness = Visual::HUD::DAMAGE_FLASH_BASE_THICKNESS * (1.0f - ease);
		float offset = Visual::HUD::DAMAGE_FLASH_OFFSET_MAX * ease;

		int color = GetColor(Visual::HUD::COLOR_DAMAGE_FLASH.r, Visual::HUD::COLOR_DAMAGE_FLASH.g, Visual::HUD::COLOR_DAMAGE_FLASH.b);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);

		int t = static_cast<int>(thickness);
		int o = static_cast<int>(offset);

		DrawBox(0, -o, System::Window::WINDOW_WIDTH, t - o, color, TRUE);
		DrawBox(0, System::Window::WINDOW_HEIGHT - t + o, System::Window::WINDOW_WIDTH, System::Window::WINDOW_HEIGHT + o, color, TRUE);
		DrawBox(-o, t - o, t - o, System::Window::WINDOW_HEIGHT - t + o, color, TRUE);
		DrawBox(System::Window::WINDOW_WIDTH - t + o, t - o, System::Window::WINDOW_WIDTH + o, System::Window::WINDOW_HEIGHT - t + o, color, TRUE);

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

			int iconSize = spec->visual.uiPath.empty() ? 0 : Visual::HUD::PICKUP_ICON_SIZE;
			int gap = (iconSize > 0) ? Visual::HUD::PICKUP_ICON_TEXT_GAP : 0;

			int contentWidth = iconSize + gap + textMaxWidth;
			int boxWidth = contentWidth + Visual::HUD::PICKUP_PADDING_X * 2;
			int boxHeight = (iconSize > 0) ? iconSize + Visual::HUD::PICKUP_PADDING_Y * 2 : Visual::ItemUI::PICKUP_BOX_HEIGHT_NO_ICON;

			int boxStartX = System::Window::CENTER_X - (boxWidth / 2);
			int boxStartY = System::Window::CENTER_Y + Visual::HUD::PICKUP_BOX_START_Y_OFFSET;

			SetDrawBlendMode(DX_BLENDMODE_ALPHA, Visual::HUD::PICKUP_PANEL_ALPHA);
			DrawBox(boxStartX, boxStartY, boxStartX + boxWidth, boxStartY + boxHeight, GetColor(Global::Palette::BLACK.r, Global::Palette::BLACK.g, Global::Palette::BLACK.b), TRUE);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

			int currentX = boxStartX + Visual::HUD::PICKUP_PADDING_X;
			int white = GetColor(Visual::HUD::COLOR_HUD_WHITE.r, Visual::HUD::COLOR_HUD_WHITE.g, Visual::HUD::COLOR_HUD_WHITE.b);

			if (iconSize > 0) {
				int iconHandle = ResourceManager::GetIns().GetGraph(spec->visual.uiPath);
				if (iconHandle != -1) {
					SetDrawBright(255, 255, 255);
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, Visual::HUD::PICKUP_ICON_PANEL_ALPHA);
					DrawExtendGraph(currentX, boxStartY + Visual::HUD::PICKUP_PADDING_Y, currentX + iconSize, boxStartY + Visual::HUD::PICKUP_PADDING_Y + iconSize, iconHandle, TRUE);
					SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
				}
				currentX += iconSize + gap;
			}

			int textStartY = boxStartY + Visual::HUD::PICKUP_PADDING_Y + (iconSize / 2) + Visual::HUD::PICKUP_TEXT_Y_OFFSET_FROM_ICON_CENTER;
			if (iconSize == 0) textStartY = boxStartY + Visual::HUD::PICKUP_TEXT_Y_OFFSET_NO_ICON;

			::DrawStringToHandle(currentX, textStartY, weaponName, white, fontJpMedium);
			::DrawStringToHandle(currentX, textStartY + Visual::HUD::PICKUP_SUBTEXT_Y_OFFSET, pickUpStr.c_str(), white, fontEnSmall);
		}
	}

	int hp = pplayer->GetHP();
	int maxHP = pplayer->GetStatus().maxHP;
	float hpRatio = static_cast<float>(hp) / maxHP;
	if (hpRatio < 0.0f) hpRatio = 0.0f;

	int hpStartX = System::Window::CENTER_X - (Visual::HUD::HP_BAR_WIDTH / 2);
	int hpStartY = System::Window::WINDOW_HEIGHT - Visual::HUD::HP_BAR_MARGIN_BOTTOM;

	DrawBox(hpStartX, hpStartY, hpStartX + Visual::HUD::HP_BAR_WIDTH, hpStartY + Visual::HUD::HP_BAR_HEIGHT, GetColor(Visual::HUD::COLOR_HP_BAR_BG.r, Visual::HUD::COLOR_HP_BAR_BG.g, Visual::HUD::COLOR_HP_BAR_BG.b), TRUE);

	int hpColor = (hpRatio > Visual::HUD::HP_LOW_RATIO_THRESHOLD) ? GetColor(Visual::HUD::COLOR_HUD_WHITE.r, Visual::HUD::COLOR_HUD_WHITE.g, Visual::HUD::COLOR_HUD_WHITE.b) : GetColor(Visual::HUD::COLOR_HUD_RED.r, Visual::HUD::COLOR_HUD_RED.g, Visual::HUD::COLOR_HUD_RED.b);
	DrawBox(hpStartX, hpStartY, hpStartX + static_cast<int>(Visual::HUD::HP_BAR_WIDTH * hpRatio), hpStartY + Visual::HUD::HP_BAR_HEIGHT, hpColor, TRUE);

	Weapon* weapon = pplayer->GetWeapon();

	if (weapon) {
		const GunStatus& spec = weapon->GetSpec();

		if (!weapon->TakingAim() || spec.id == WeaponID::LR || spec.id == WeaponID::SMG) {
			DrawCircle(System::Window::CENTER_X, System::Window::CENTER_Y, Visual::HUD::CROSSHAIR_DOT_SIZE, GetColor(Visual::HUD::COLOR_HUD_WHITE.r, Visual::HUD::COLOR_HUD_WHITE.g, Visual::HUD::COLOR_HUD_WHITE.b), TRUE);
		}
		int ammo = weapon->GetAmmo();
		int mag = spec.magAmmo;
		int reserve = weapon->GetReserveAmmo();
		const char* weaponName = TextManager::GetIns().GetWeaponName(spec.id);

		int uiBaseX = System::Window::WINDOW_WIDTH - Visual::HUD::WEAPON_ICON_SIZE - Visual::HUD::WEAPON_UI_MARGIN_RIGHT;
		int uiBaseY = System::Window::WINDOW_HEIGHT - Visual::HUD::WEAPON_ICON_SIZE - Visual::HUD::WEAPON_UI_MARGIN_BOTTOM;

		if (!spec.visual.uiPath.empty()) {
			int iconHandle = ResourceManager::GetIns().GetGraph(spec.visual.uiPath);
			if (iconHandle != -1) {
				SetDrawBright(0, 0, 0);
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, Visual::HUD::WEAPON_ICON_PANEL_ALPHA);
				DrawExtendGraph(uiBaseX, uiBaseY, uiBaseX + Visual::HUD::WEAPON_ICON_SIZE, uiBaseY + Visual::HUD::WEAPON_ICON_SIZE, iconHandle, TRUE);
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
				SetDrawBright(255, 255, 255);
			}
		}

		int textX = uiBaseX + Visual::HUD::WEAPON_TEXT_PADDING_X;
		int textY = uiBaseY + Visual::HUD::WEAPON_ICON_SIZE - Visual::HUD::WEAPON_TEXT_Y_OFFSET_FROM_ICON_BOTTOM;

		::DrawFormatStringToHandle(textX, textY, GetColor(Visual::HUD::COLOR_HUD_WHITE.r, Visual::HUD::COLOR_HUD_WHITE.g, Visual::HUD::COLOR_HUD_WHITE.b), fontJpMedium, "%s", weaponName);
		::DrawFormatStringToHandle(textX, textY + Visual::HUD::WEAPON_AMMO_TEXT_Y_OFFSET, GetColor(Visual::HUD::COLOR_HUD_WHITE.r, Visual::HUD::COLOR_HUD_WHITE.g, Visual::HUD::COLOR_HUD_WHITE.b), fontEnLarge, "AMMO  %02d / %02d", ammo, mag);

		if (!weapon->IsInfinite()) {
			::DrawFormatStringToHandle(textX, textY + Visual::HUD::WEAPON_RESERVE_TEXT_Y_OFFSET, GetColor(Visual::HUD::COLOR_RESERVE_AMMO_TEXT.r, Visual::HUD::COLOR_RESERVE_AMMO_TEXT.g, Visual::HUD::COLOR_RESERVE_AMMO_TEXT.b), fontEnSmall, "      %03d", reserve);
		}

		if (weapon->Reloading()) {
			float prog = weapon->GetReloadProgress();
			int cx = System::Window::CENTER_X;
			int cy = System::Window::CENTER_Y;

			DrawCircle(cx, cy, Visual::HUD::RELOAD_CIRCLE_RADIUS, GetColor(Visual::HUD::COLOR_RELOAD_CIRCLE_BG.r, Visual::HUD::COLOR_RELOAD_CIRCLE_BG.g, Visual::HUD::COLOR_RELOAD_CIRCLE_BG.b), FALSE);

			int arcColor = GetColor(Visual::HUD::COLOR_HUD_YELLOW.r, Visual::HUD::COLOR_HUD_YELLOW.g, Visual::HUD::COLOR_HUD_YELLOW.b);
			for (int i = 0; i < static_cast<int>(Visual::HUD::RELOAD_CIRCLE_SEGMENTS * prog); ++i) {
				float a1 = -Global::Math::MATH_PI_HALF + (DX_PI_F * 2.0f * i / Visual::HUD::RELOAD_CIRCLE_SEGMENTS);
				float a2 = -Global::Math::MATH_PI_HALF + (DX_PI_F * 2.0f * (i + 1) / Visual::HUD::RELOAD_CIRCLE_SEGMENTS);

				int x1 = cx + static_cast<int>(cosf(a1) * Visual::HUD::RELOAD_CIRCLE_RADIUS);
				int y1 = cy + static_cast<int>(sinf(a1) * Visual::HUD::RELOAD_CIRCLE_RADIUS);
				int x2 = cx + static_cast<int>(cosf(a2) * Visual::HUD::RELOAD_CIRCLE_RADIUS);
				int y2 = cy + static_cast<int>(sinf(a2) * Visual::HUD::RELOAD_CIRCLE_RADIUS);

				DrawLine(x1, y1, x2, y2, arcColor, Visual::HUD::RELOAD_CIRCLE_LINE_THICKNESS);
			}

			const char* reloadText = "RELOADING";
			int rWidth = GetDrawStringWidthToHandle(reloadText, static_cast<int>(strlen(reloadText)), fontEnSmall);
			::DrawStringToHandle(cx - (rWidth / 2), cy + Visual::HUD::RELOAD_CIRCLE_RADIUS + Visual::HUD::RELOAD_TEXT_Y_OFFSET, reloadText, arcColor, fontEnSmall);
		}

		const auto& attackers = pplayer->GetTargeted();
		if (!attackers.empty()) {
			float playerYaw = pplayer->GetCam()->GetYaw() * Global::Math::DEG_TO_RAD;

			for (const auto& info : attackers) {
				VECTOR toEnemy = VSub(info.pos, pplayer->GetPos());
				float enemyAngle = atan2f(toEnemy.x, toEnemy.z);
				float relativeAngle = enemyAngle - playerYaw;
				float prog = info.progress;

				float radius = Visual::HUD::INDICATOR_RADIUS + Visual::HUD::INDICATOR_RADIUS_INCREASE * (1.0f - prog);
				float arcAngle = Visual::HUD::INDICATOR_ARC_ANGLE_SCALE * (1.0f - prog) + Visual::HUD::INDICATOR_ARC_ANGLE_OFFSET;

				int color = GetColor(255, static_cast<int>(255 * (1.0f - prog)), 0);
				int alpha = static_cast<int>(Visual::HUD::INDICATOR_ALPHA_MIN + Visual::HUD::INDICATOR_ALPHA_RANGE * prog);

				SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
				for (int i = 0; i < Visual::HUD::INDICATOR_SEGMENTS; ++i) {
					float a1 = relativeAngle - arcAngle + (arcAngle * 2.0f * i / Visual::HUD::INDICATOR_SEGMENTS);
					float a2 = relativeAngle - arcAngle + (arcAngle * 2.0f * (i + 1) / Visual::HUD::INDICATOR_SEGMENTS);

					int x1 = System::Window::CENTER_X + static_cast<int>(sinf(a1) * radius);
					int y1 = System::Window::CENTER_Y - static_cast<int>(cosf(a1) * radius);
					int x2 = System::Window::CENTER_X + static_cast<int>(sinf(a2) * radius);
					int y2 = System::Window::CENTER_Y - static_cast<int>(cosf(a2) * radius);

					DrawLine(x1, y1, x2, y2, color, Visual::HUD::INDICATOR_LINE_THICKNESS);
				}
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			}
		}
	}
}

void HUD::OnHitTarget(bool isHeadShot,bool isKill) {
	hitMarkTimer = Visual::HUD::HITMARK_DURATION;
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