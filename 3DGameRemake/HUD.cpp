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
	int textWidth = GetDrawStringWidth(topTextBuf, static_cast<int>(strlen(topTextBuf)));
	int drawX = CENTER_X - (textWidth / 2);


	if (prepareTime > 0.0f) {
		DrawString(drawX, topY, topTextBuf, textColor);
	}
	else {
		int alpha = static_cast<int>(scoreFadeAlpha * 255);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
		DrawString(drawX, topY, topTextBuf, textColor);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}

	if (hitMarkTimer > 0.0f) {
		// 定数（必要に応じて cpp の上部 anonymous namespace に移動させても構いません）
		const float HITMARK_DURATION = 0.2f;

		// 進行度とイージング（徐々に減速しながら広がる Cubic Ease-Out）
		float progress = 1.0f - (hitMarkTimer / HITMARK_DURATION); // 0.0 -> 1.0
		float easeOut = 1.0f - std::powf(1.0f - progress, 3.0f);

		int cx = CENTER_X;
		int cy = CENTER_Y;

		// 通常ヒットは白、ヘッドショット時は赤色に設定
		int baseColor = lastHitWasHS ? GetColor(255, 50, 50) : GetColor(255, 255, 255);

		// 線の太さ（HS時は太くする）
		int thickness = lastHitWasHS ? 4 : 2;

		// 線の長さ（HS時は長くする）
		float lineLength = lastHitWasHS ? 14.0f : 8.0f;

		// 透明度（スッと消える）
		int alpha = static_cast<int>(255 * (1.0f - easeOut));

		// 外側へ弾けるオフセット計算（HS時の方がより遠くへ弾ける）
		float baseOffset = 8.0f; // 中心からの初期距離
		float expandOffset = baseOffset + (lastHitWasHS ? 20.0f : 12.0f) * easeOut;

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);

		// 4本の斜め線を計算して描画（外側に向かって移動する）
		// 左上
		DrawLine(cx - static_cast<int>(expandOffset + lineLength), cy - static_cast<int>(expandOffset + lineLength),
			cx - static_cast<int>(expandOffset), cy - static_cast<int>(expandOffset), baseColor, thickness);
		// 右上
		DrawLine(cx + static_cast<int>(expandOffset + lineLength), cy - static_cast<int>(expandOffset + lineLength),
			cx + static_cast<int>(expandOffset), cy - static_cast<int>(expandOffset), baseColor, thickness);
		// 左下
		DrawLine(cx - static_cast<int>(expandOffset + lineLength), cy + static_cast<int>(expandOffset + lineLength),
			cx - static_cast<int>(expandOffset), cy + static_cast<int>(expandOffset), baseColor, thickness);
		// 右下
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
		DrawBox(0, WINDOW_HEIGHT - t + o, WINDOW_WIDTH, WINDOW_HEIGHT + o, color, TRUE); // 下
		DrawBox(-o, t - o, t - o, WINDOW_HEIGHT - t + o, color, TRUE); // 左
		DrawBox(WINDOW_WIDTH - t + o, t - o, WINDOW_WIDTH + o, WINDOW_HEIGHT - t + o, color, TRUE); // 右

		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
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
	float hpRatio = static_cast<float>(hp) / maxHP;
	if (hpRatio < 0.0f) hpRatio = 0.0f;

	int hpBarWidth = 600;
	int hpBarHeight = 10;
	int hpStartX = CENTER_X - (hpBarWidth / 2);
	int hpStartY = WINDOW_HEIGHT - 30;

	// 背景の暗いライン
	DrawBox(hpStartX, hpStartY, hpStartX + hpBarWidth, hpStartY + hpBarHeight, GetColor(40, 40, 40), TRUE);

	// 現在のHPライン（通常は白、ピンチ時(30%以下)は赤）
	int hpColor = (hpRatio > 0.3f) ? GetColor(255, 255, 255) : GetColor(255, 50, 50);
	DrawBox(hpStartX, hpStartY, hpStartX + static_cast<int>(hpBarWidth * hpRatio), hpStartY + hpBarHeight, hpColor, TRUE);

	Weapon* weapon = pplayer->GetWeapon();

	if (weapon) {
		if (!weapon->TakingAim()) {
			DrawCircle(CENTER_X, CENTER_Y, CROSSHAIR_DOT_SIZE, GetColor(0, 0, 0), true);
		}

		const GunStatus& spec = weapon->GetSpec();
		int ammo = weapon->GetAmmo();
		int mag = spec.magAmmo;
		int reserve = weapon->GetReserveAmmo();
		const char* weaponName = TextManager::GetIns().GetWeaponName(spec.id);

		int iconSize = 250;
		int uiBaseX = WINDOW_WIDTH - iconSize - 30;
		int uiBaseY = WINDOW_HEIGHT - iconSize - 20;

		// 1. 武器画像が存在する場合のみ、背景（ウォーターマーク）としてグレー・半透明で描画
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

		DrawFormatString(textX, textY, GetColor(255, 255, 255), "%s", weaponName);
		DrawFormatString(textX, textY + 30, GetColor(255, 255, 255), "AMMO  %02d / %02d", ammo, mag);

		if (!weapon->IsInfinite()) {
			DrawFormatString(textX, textY + 55, GetColor(200, 200, 200), "      %03d", reserve);
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

			// RELOADING の文字は、視界中心の邪魔にならないよう円グラフの下に少し離して配置
			const char* reloadText = "RELOADING";
			int rWidth = GetDrawStringWidth(reloadText, static_cast<int>(strlen(reloadText)));
			DrawString(cx - (rWidth / 2), cy + radius + 15, reloadText, arcColor);
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

void HUD::OnHitTarget(bool isHeadShot) {
	hitMarkTimer = HITMARK_DURATION;
	lastHitWasHS = isHeadShot;
}

void HUD::OnPlayerTakeDamage() {
	damageFlashTimer = DAMAGE_FLASH_DURATION;
}