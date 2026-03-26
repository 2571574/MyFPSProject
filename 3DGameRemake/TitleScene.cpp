#include "TitleScene.h"
#include "TutorialScene.h"
#include "GameScene.h"
#include "InputManager.h"
#include "ConfigManager.h"
#include "TextManager.h"
#include "ResourceManager.h"
#include "SoundManager.h"
#include "Debug.h"

#include <memory>
#include<algorithm>

namespace {
	constexpr float INTRO_DURATION = 0.5f;
	constexpr float SLIDE_START_X = -500.0f;

	constexpr int MENU_BASE_X = 150;		// メニューの基準X座標
	constexpr int MENU_BASE_Y = 200;		// メニューの基準Y座標
	constexpr int MENU_LINE_HEIGHT = 110;	// 行間
	constexpr int MENU_SLANT_X = 40;

	constexpr int MENU_BG_WIDTH = 400;   // 背景の横幅
	constexpr int MENU_BG_HEIGHT = 75;  // 背景の高さ
	constexpr int MENU_BG_SLANT = 35;   // 平行四辺形自体の傾斜量
	constexpr int MENU_BG_OFFSET_X = -80; // テキストに対する背景のXオフセット
	constexpr int MENU_BG_OFFSET_Y = -10; // テキストに対する背景のYオフセット

	constexpr int KEY_MAX = 256;
	constexpr int BUTTON_MAX = 0x8000;
	constexpr float BG_SCROLL_X = 50.0f;
	constexpr float BOBSPEED = 10.0f;

	constexpr int DIALOG_X1 = CENTER_X + 50;
	constexpr int DIALOG_Y1 = CENTER_Y + 100;
	constexpr int DIALOG_X2 = WINDOW_WIDTH - 50;
	constexpr int DIALOG_Y2 = WINDOW_HEIGHT - 100;
}
//キーバインド可能なアクション
static const std::vector<ActionID> GAMEPLAY_ACTION = {
	ActionID::MOVE_FORWARD,ActionID::MOVE_LEFT,ActionID::MOVE_BACK,ActionID::MOVE_RIGHT,
	ActionID::FIRE,ActionID::ADS,ActionID::RELOAD,ActionID::RUN,ActionID::JUMP,ActionID::CROUCH,ActionID::WEAPON_NEXT,
	ActionID::WEAPON_PREV,ActionID::INTERACT
};


TitleScene::TitleScene(SceneManager* manager) :BaseScene(manager)
, selectNum(0), currentState(TitleState::TOP)
{}

TitleScene::~TitleScene() {

}


void TitleScene::Init() {
	titleLogoHandle = ResourceManager::GetIns().GetGraph("Resource/titleLogo.png");
	titleBG = ResourceManager::GetIns().GetGraph("Resource/titlebg.png");
fontMenuLarge = ResourceManager::GetIns().GetFont("Resource/Font/JetBrainsMono_40.dft");
	fontMenuSmall = ResourceManager::GetIns().GetFont("Resource/Font/NotoSansJP_28.dft");
	fontDesc = ResourceManager::GetIns().GetFont("Resource/Font/NotoSansJP_22.dft");
	introTimer = 0.0f;
	SoundManager::GetIns().PlayBGM("Resource/Sound/TitleBGM.mp3");
	ChangeState(TitleState::TOP);
}


void TitleScene::Update() {
	Control();
	float dt = Time::GetIns().GetDelta();
	if (introTimer < INTRO_DURATION) {
		introTimer += dt;
	}
	BGscrollX += BG_SCROLL_X * dt;
	if (selectNum < 0) selectNum = 0;
	switch (currentState) {
	case TitleState::TOP:

		if (selectNum >= MENU_MAX) {
			selectNum = MENU_MAX - 1;
		}
		if (InputManager::GetIns().IsActionTrigger(ActionID::MENU_SELECT)) {
			SoundManager::GetIns().PlaySE("Resource/Sound/select.ogg");
			if (selectNum == MENU_PLAY) {
				ChangeState(TitleState::MODE_SELECT);
				
			}
			else if (selectNum == MENU_SETTING) {
				ChangeState(TitleState::SETTINGS);
			}
			else if (selectNum == MENU_CREDIT) {
				ChangeState(TitleState::CREDIT);
			
			}
			else if (selectNum == MENU_EXIT) {
				manager->SetExitTag(true);
			}
		}
		break;

	case TitleState::MODE_SELECT:
		if (selectNum >= (int)PlayMode::MODE_MAX) {
			selectNum = (int)PlayMode::MODE_MAX - 1;
		}
		if (InputManager::GetIns().IsActionTrigger(ActionID::MENU_BACK)) {
			SoundManager::GetIns().PlaySE("Resource/Sound/back.ogg");
			ChangeState(TitleState::TOP);
		}
		if (InputManager::GetIns().IsActionTrigger(ActionID::MENU_SELECT)) {
			SoundManager::GetIns().PlaySE("Resource/Sound/select.ogg");
			manager->SetCurrentMode((PlayMode)selectNum);
			if ((PlayMode)selectNum == PlayMode::MODE_TUTORIAL) {
				manager->ChangeScene(std::make_unique<TutorialScene>(manager));
			}
			else {
				manager->ChangeScene(std::make_unique<GameScene>(manager));
			}
		}
		break;

	case TitleState::SETTINGS:
		if (InputManager::GetIns().IsActionTrigger(ActionID::MENU_LEFT)) {
			SoundManager::GetIns().PlaySE("Resource/Sound/cursormove.ogg");
			ModifySetting((SettingItem)selectNum, -1);
		}
		if (InputManager::GetIns().IsActionTrigger(ActionID::MENU_RIGHT)) {
			SoundManager::GetIns().PlaySE("Resource/Sound/cursormove.ogg");
			ModifySetting((SettingItem)selectNum, 1);
		}
		if (InputManager::GetIns().IsActionTrigger(ActionID::MENU_BACK)) {
			SoundManager::GetIns().PlaySE("Resource/Sound/back.ogg");
			ChangeState(TitleState::TOP);
		}
		if (InputManager::GetIns().IsActionTrigger(ActionID::MENU_SELECT)) {
			SoundManager::GetIns().PlaySE("Resource/Sound/select.ogg");
			if ((SettingItem)selectNum == SettingItem::KEY_CONFIG) {
				ChangeState(TitleState::KEY_CONFIG);
			}
			else if ((SettingItem)selectNum == SettingItem::BACK) {
				ChangeState(TitleState::TOP);
			}
		}
		break;


	case TitleState::KEY_CONFIG:
		//キー入力を受け取る
		if (isWaitingKey) {
			int newCode = -1;
			InputType detectedType = InputType::KEYBOARD;
			if (columnidx == 0) {
				for (int i = 0; i < KEY_MAX; i++) {
					//キーボードのコードを受け取る
					if (CheckKey::GetIns().isPress(InputType::KEYBOARD,i)) {
						newCode = i;
						detectedType = InputType::KEYBOARD;
						break;
					}
					//エラーだった場合、マウスの入力かどうかチェック
					if (newCode == -1) {
						if (CheckKey::GetIns().isPress(InputType::MOUSE, MOUSE_INPUT_LEFT)) {
							newCode = MOUSE_INPUT_LEFT; detectedType = InputType::MOUSE;
						}
						if (CheckKey::GetIns().isPress(InputType::MOUSE, MOUSE_INPUT_RIGHT)) {
							newCode = MOUSE_INPUT_RIGHT; detectedType = InputType::MOUSE;
						}
						if (CheckKey::GetIns().isPress(InputType::MOUSE, MOUSE_INPUT_MIDDLE)) {
							newCode = MOUSE_INPUT_MIDDLE; detectedType = InputType::MOUSE;
						}
						if (CheckKey::GetIns().isPress(InputType::MOUSE, MOUSE_WHEEL_UP)) {
							newCode = MOUSE_WHEEL_UP; detectedType = InputType::MOUSE;
						}
						if (CheckKey::GetIns().isPress(InputType::MOUSE, MOUSE_WHEEL_DOWN)) {
							newCode = MOUSE_WHEEL_DOWN; detectedType = InputType::MOUSE;
						}
						if (CheckKey::GetIns().isPress(InputType::MOUSE, MOUSE_INPUT_4)) {
							newCode = MOUSE_INPUT_4; detectedType = InputType::MOUSE;
						}
						if (CheckKey::GetIns().isPress(InputType::MOUSE, MOUSE_INPUT_5)) {
							newCode = MOUSE_INPUT_5; detectedType = InputType::MOUSE;
						}
					}
				}
			}
			//右列はコントローラーの入力としてチェック
			else if (columnidx == 1) {
				for (int i = 1; i <= BUTTON_MAX; i <<= 1) {
					if (CheckKey::GetIns().isPress(InputType::JOY, i)) {
						newCode = i; 
						detectedType = InputType::JOY; 
						break;
					}
				}
			}

			//受け取ったキー入力で更新する
			if (newCode != -1) {
				RemoveDuplicateBind(detectedType, newCode);

				ActionID target = GAMEPLAY_ACTION[selectNum];
				auto& targetBinds = ConfigManager::GetIns().Bindings()[target];

				bool updated = false;
				for (auto& b : targetBinds) {
					if (b.type == detectedType) {
						b.KeyCode = newCode; 
						updated = true;
						break;
					}
				}
				if (!updated)targetBinds.push_back({ detectedType,newCode });

				InputManager::GetIns().SetBind(ConfigManager::GetIns().Bindings());
				ConfigManager::GetIns().Save();
				isWaitingKey = false;
			}
		}

		//メニュー操作
		else {
			if (InputManager::GetIns().IsActionTrigger(ActionID::MENU_LEFT)) {
				columnidx = 0;
				SoundManager::GetIns().PlaySE("Resource/Sound/cursormove.ogg");
			}

			if (InputManager::GetIns().IsActionTrigger(ActionID::MENU_RIGHT)) {
				ActionID target = GAMEPLAY_ACTION[selectNum];
				bool isMoveAction = (target == ActionID::MOVE_FORWARD || target == ActionID::MOVE_LEFT ||
					target == ActionID::MOVE_BACK || target == ActionID::MOVE_RIGHT);
				if (!isMoveAction) {
					SoundManager::GetIns().PlaySE("Resource/Sound/cursormove.ogg");
					columnidx = 1;
				}
			}

			if (InputManager::GetIns().IsActionTrigger(ActionID::MENU_BACK)) {
				SoundManager::GetIns().PlaySE("Resource/Sound/back.ogg");
				ChangeState(TitleState::SETTINGS);
				selectNum = (int)SettingItem::KEY_CONFIG;
			}
			if (InputManager::GetIns().IsActionTrigger(ActionID::MENU_SELECT)) {
				SoundManager::GetIns().PlaySE("Resource/Sound/select.ogg");
				ActionID target = GAMEPLAY_ACTION[selectNum];
				bool isMoveAction = (target == ActionID::MOVE_FORWARD || target == ActionID::MOVE_LEFT ||
					target == ActionID::MOVE_BACK || target == ActionID::MOVE_RIGHT);

				if (columnidx == 1 && isMoveAction) {
					// コントローラー列の移動アクション選択時は何もしない（一応残しておきます）
				}
				else {
					isWaitingKey = true;
				}
			}
		}
		break;

	case TitleState::CREDIT:
		if (InputManager::GetIns().IsActionTrigger(ActionID::MENU_BACK)) {
			SoundManager::GetIns().PlaySE("Resource/Sound/back.ogg");
			ChangeState(TitleState::TOP);
		}
		break;

	}
}

void TitleScene::Draw() {
	const int BASE_X = 100;
	const int CHOOSE_X = BASE_X - 20;
	const int BASE_Y = 100;
	const int LINE_HEIGHT = 30;
	const int LOGO_X = WINDOW_WIDTH - 200;


	

	if (titleBG != -1) {
		int bgWidth, bgHeight;
		GetGraphSize(titleBG, &bgWidth, &bgHeight);

		int drawOffset = (int)BGscrollX % bgWidth;
		for (int x = -drawOffset; x < WINDOW_WIDTH; x += bgWidth) {
			::SetDrawBlendMode(DX_BLENDMODE_ALPHA, 100);
			DrawGraph(x, 0, titleBG, true);
			::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		}
		if (currentState == TitleState::SETTINGS ||
			currentState == TitleState::KEY_CONFIG ||
			currentState == TitleState::CREDIT) {
			::SetDrawBlendMode(DX_BLENDMODE_ALPHA, 200);
			::DrawBox(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, ::GetColor(0, 0, 0), TRUE);
			::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		}
	}


	float rawProgress = introTimer / INTRO_DURATION;
	float progress = (rawProgress > 1.0f) ? 1.0f : rawProgress;

	float easeOut = 1.0f - std::pow(1.0f - progress, 3.0f);
	float slideX = (1.0f - easeOut) * SLIDE_START_X;

	auto DrawGeometricBG = [&](int x, int y, int color, int alpha) {
		::SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
		// 4つの頂点を計算（右に傾いた平行四辺形）
		int x1 = x;
		int y1 = y;
		int x2 = x + MENU_BG_WIDTH;
		int y2 = y;
		int x3 = x + MENU_BG_WIDTH + MENU_BG_SLANT;
		int y3 = y + MENU_BG_HEIGHT;
		int x4 = x + MENU_BG_SLANT;
		int y4 = y + MENU_BG_HEIGHT;

		// 三角形2つで四角形を構成
		DrawTriangle(x1, y1, x2, y2, x3, y3, color, TRUE);
		DrawTriangle(x1, y1, x3, y3, x4, y4, color, TRUE);
		::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		};

	switch (currentState) {
	case TitleState::TOP: {
		// ロゴ描画（既存）
		if (titleLogoHandle != -1) {
			::SetDrawBlendMode(DX_BLENDMODE_ADD, 255);
			DrawExtendGraph(WINDOW_WIDTH - 800, 100, WINDOW_WIDTH - 300, 600, titleLogoHandle, true);
			::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		}

		// 選択肢の描画
		const char* menuLabels[MENU_MAX] = { "PLAY", "SETTINGS", "CREDIT", "EXIT" };
		for (int i = 0; i < MENU_MAX; i++) {
			int drawX = static_cast<int>(MENU_BASE_X + (i * MENU_SLANT_X) + slideX);
			int drawY = MENU_BASE_Y + (i * MENU_LINE_HEIGHT);

			bool isSelected = (i == selectNum);
			int bgColor = GetColor(255, 255, 255);

			// 選択中は不透明な白、非選択時は極めて薄い白（ガイドラインとして機能）
			int bgAlpha = isSelected ? 255 : 40;
			// ★ 選択中は背景が白なので、文字を黒にする（視認性の確保）
			int textColor = isSelected ? GetColor(0, 0, 0) : GetColor(255, 255, 255);

			// 背景描画
			DrawGeometricBG(drawX + MENU_BG_OFFSET_X, drawY + MENU_BG_OFFSET_Y, bgColor, bgAlpha);

			::DrawStringToHandle(drawX, drawY, menuLabels[i], textColor,fontMenuLarge);
		}

		// 説明ボックスの描画
		::SetDrawBlendMode(DX_BLENDMODE_ALPHA, 100);
		::DrawBox(DIALOG_X1, DIALOG_Y1, DIALOG_X2, DIALOG_Y2, GetColor(255, 255, 255), true);
		::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		
		const char* description = TextManager::GetIns().GetMenuDescription(selectNum);
		::DrawStringToHandle(DIALOG_X1 + 30, DIALOG_Y1 + 30, description, GetColor(0, 0, 0),fontDesc);
		break;
	}
	case TitleState::MODE_SELECT: {
		const char* modeLabels[] = { "TUTORIAL", "EASY", "NORMAL", "HARD" };
		int modeMax = static_cast<int>(PlayMode::MODE_MAX);

		for (int i = 0; i < modeMax; i++) {
			int drawX = static_cast<int>(MENU_BASE_X + (i * MENU_SLANT_X) + slideX);
			int drawY = MENU_BASE_Y + (i * MENU_LINE_HEIGHT);

			bool isSelected = (i == selectNum);
			int bgColor = GetColor(255, 255, 255);

			int bgAlpha = isSelected ? 255 : 40;
			int textColor = isSelected ? GetColor(0, 0, 0) : GetColor(255, 255, 255);

			// 背景描画
			DrawGeometricBG(drawX + MENU_BG_OFFSET_X, drawY + MENU_BG_OFFSET_Y, bgColor, bgAlpha);

			// テキスト描画
			::DrawStringToHandle(drawX, drawY, modeLabels[i], textColor, fontMenuLarge);
		}

		// 説明ボックスの描画
		::SetDrawBlendMode(DX_BLENDMODE_ALPHA, 100);
		::DrawBox(DIALOG_X1, DIALOG_Y1, DIALOG_X2, DIALOG_Y2, GetColor(255, 255, 255), true);
		::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

		const char* description = TextManager::GetIns().GetMenuDescription(selectNum + 13);
		::DrawStringToHandle(DIALOG_X1 + 30, DIALOG_Y1 + 30, description, GetColor(0, 0, 0), fontDesc);
		break;
	}
	case TitleState::SETTINGS:
		DrawSettings();
		break;
	case TitleState::CREDIT: {
		const int BASE_X = 150;
		const int TITLE_Y = 100;
		const int START_Y = 200;
		const int LINE_HEIGHT = 45; // 行間
		const int SECTION_GAP = 30; // セクションごとの隙間
		const int LINK_X_OFFSET = 350; // サイト名からリンクまでのX距離

		int white = GetColor(255, 255, 255);
		int yellow = GetColor(255, 255, 0);
		int linkColor = GetColor(150, 200, 255);

		// タイトル
		::DrawStringToHandle(BASE_X, TITLE_Y, "CREDIT", white, fontMenuLarge);

		int currentY = START_Y;

		// --- 【3Dモデル】 ---
		::DrawStringToHandle(BASE_X, currentY, "< モデル >", yellow, fontMenuSmall);
		currentY += LINE_HEIGHT;
		::DrawStringToHandle(BASE_X + 40, currentY, "・Quaternius", white, fontDesc);
		::DrawStringToHandle(BASE_X + LINK_X_OFFSET, currentY, "https://quaternius.com/", linkColor, fontDesc);
		currentY += LINE_HEIGHT + SECTION_GAP;


		// --- 【サウンド (BGM・効果音)】 ---
		::DrawStringToHandle(BASE_X, currentY, "< サウンド >", yellow, fontMenuSmall);
		currentY += LINE_HEIGHT;
		// BGM
		::DrawStringToHandle(BASE_X + 40, currentY, "・PeriTune", white, fontDesc);
		::DrawStringToHandle(BASE_X + LINK_X_OFFSET, currentY, "https://peritune.com/", linkColor, fontDesc);
		currentY += LINE_HEIGHT;
		::DrawStringToHandle(BASE_X + 40, currentY, "・Pixabay", white, fontDesc);
		::DrawStringToHandle(BASE_X + LINK_X_OFFSET, currentY, "https://pixabay.com/", linkColor, fontDesc);
		currentY += LINE_HEIGHT;
		// SE
		::DrawStringToHandle(BASE_X + 40, currentY, "・On-Jin ～音人～", white, fontDesc);
		::DrawStringToHandle(BASE_X + LINK_X_OFFSET, currentY, "https://on-jin.com/", linkColor, fontDesc);
		currentY += LINE_HEIGHT;
		::DrawStringToHandle(BASE_X + 40, currentY, "・Kenney", white, fontDesc);
		::DrawStringToHandle(BASE_X + LINK_X_OFFSET, currentY, "https://kenney.nl/", linkColor, fontDesc);
		currentY += LINE_HEIGHT + SECTION_GAP;


		// --- 【フォント】 ---
		::DrawStringToHandle(BASE_X, currentY, "< フォント >", yellow, fontMenuSmall);
		currentY += LINE_HEIGHT;
		::DrawStringToHandle(BASE_X + 40, currentY, "・Google Fonts", white, fontDesc);
		::DrawStringToHandle(BASE_X + LINK_X_OFFSET, currentY, "https://fonts.google.com/", linkColor, fontDesc);
		currentY += LINE_HEIGHT + SECTION_GAP;


		// --- 【開発ツール・UI画像】 ---
		::DrawStringToHandle(BASE_X, currentY, "< 開発ツール・画像 >", yellow, fontMenuSmall);
		currentY += LINE_HEIGHT;
		::DrawStringToHandle(BASE_X + 40, currentY, "・Unity (マップ制作)", white, fontDesc);
		currentY += LINE_HEIGHT;
		::DrawStringToHandle(BASE_X + 40, currentY, "・Gemini (画像生成)", white, fontDesc);

		break;
	}
	case TitleState::KEY_CONFIG:
		DrawKeyConfig();
		break;
	}
	::DrawBox(0, WINDOW_HEIGHT - 50, WINDOW_WIDTH, WINDOW_HEIGHT, GetColor(0, 0, 0), TRUE);

	std::string menuUp = TextManager::GetIns().GetActionKeyString(ActionID::MENU_UP);
	std::string menuSel = TextManager::GetIns().GetActionKeyString(ActionID::MENU_SELECT);
	std::string menuBack = TextManager::GetIns().GetActionKeyString(ActionID::MENU_BACK);
	std::string helpText =  " 方向 : メニュー操作 | " + menuSel + " : 決定 | " + menuBack + " : 戻る";

	int textWidth = GetDrawStringWidthToHandle(helpText.c_str(), static_cast<int>(helpText.length()), fontDesc);
	::DrawStringToHandle(WINDOW_WIDTH - textWidth - 50, WINDOW_HEIGHT - 30, helpText.c_str(), GetColor(255, 255, 255), fontDesc);
}

void TitleScene::DrawSettings() {
	const int BASE_X = 150;
	const int CHOOSE_X = BASE_X - 20;
	const int BASE_Y = 200;
	const int LINE_HEIGHT = 50;

	::DrawStringToHandle(BASE_X, 100, "SETTINGS", GetColor(255, 255, 255), fontMenuLarge);
	auto& s = ConfigManager::GetIns().Settings();

	//設定項目の文字列
	const char* labels[(int)SettingItem::MAX] = {
		"BGM音量", "SE音量",
		"マウス感度", "スティック感度",
		"視野角", "画面の揺れ",
		"リコイルリカバリー", "キーバインドを編集",
		"タイトルに戻る"
	};


	for (int i = 0; i < (int)SettingItem::MAX; i++) {
		int color = (i == selectNum) ? GetColor(255, 255, 0) : GetColor(255, 255, 255);
		int drawY = BASE_Y + i * LINE_HEIGHT;

		
		::DrawStringToHandle(CHOOSE_X, drawY, (i == selectNum) ? ">" : " ", color, fontMenuSmall);

		//描画
		switch ((SettingItem)i) {
		case SettingItem::BGM_VOLUME:
			::DrawFormatStringToHandle(BASE_X, drawY, color, fontMenuSmall, "%-20s : %d%%", labels[i], (int)(s.bgmVolume * 100));
			break;
		case SettingItem::SE_VOLUME:
			::DrawFormatStringToHandle(BASE_X, drawY, color, fontMenuSmall, "%-20s : %d%%", labels[i], (int)(s.seVolume * 100));
			break;
		case SettingItem::MOUSE_SENSITIVITY:
			::DrawFormatStringToHandle(BASE_X, drawY, color, fontMenuSmall, "%-20s : %.0f", labels[i], s.mouseSensitivity * 1000.0f);
			break;
		case SettingItem::PAD_SENSITIVITY:
			::DrawFormatStringToHandle(BASE_X, drawY, color, fontMenuSmall, "%-20s : %.0f", labels[i], s.padSensitivity * 10000.0f);
			break;
		case SettingItem::BASE_FOV:
			::DrawFormatStringToHandle(BASE_X, drawY, color, fontMenuSmall, "%-20s : %.0f", labels[i], s.basefov);
			break;
		case SettingItem::HEAD_BOB:
			::DrawFormatStringToHandle(BASE_X, drawY, color, fontMenuSmall, "%-20s : %s", labels[i], s.headbob ? "ON" : "OFF");
			break;
		case SettingItem::RECOVERY:
			::DrawFormatStringToHandle(BASE_X, drawY, color, fontMenuSmall, "%-20s : %s", labels[i], s.recovery ? "ON" : "OFF");
			break;
		case SettingItem::KEY_CONFIG:
			::DrawFormatStringToHandle(BASE_X, drawY, color, fontMenuSmall, "%-20s : >>>", labels[i]);
			break;
		case SettingItem::BACK:
			::DrawFormatStringToHandle(BASE_X, drawY, color, fontMenuSmall, "%s", labels[i]);
			break;
		default:
			break;
		}
	}
	::SetDrawBlendMode(DX_BLENDMODE_ALPHA, 100);
	::DrawBox(DIALOG_X1, DIALOG_Y1, DIALOG_X2, DIALOG_Y2, GetColor(255, 255, 255), true);
	::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	const char* description = TextManager::GetIns().GetMenuDescription(selectNum + 4);
	::DrawStringToHandle(DIALOG_X1 + 30, DIALOG_Y1 + 30, description, GetColor(0, 0, 0), fontDesc);
}

void TitleScene::DrawKeyConfig() {
	
	const int BASE_X = 400;
	const int BASE_Y = 250;
	const int LINE_HEIGHT = 50;
	const int COL0_X = 900;
	const int COL1_X = 1400;

	// タイトル（左上に大きく配置）
	::DrawStringToHandle(150, 100, "Key Config", GetColor(255, 255, 255), fontMenuLarge);

	// ヘッダーテキスト（それぞれの列の真上に配置）
	::DrawStringToHandle(COL0_X, 180, "キーボード/マウス", GetColor(150, 150, 150), fontMenuSmall);
	::DrawStringToHandle(COL1_X, 180, "コントローラー", GetColor(150, 150, 150), fontMenuSmall);

	// ヘッダーの下に区切り線を引いてソリッド感を出す
	::SetDrawBlendMode(DX_BLENDMODE_ALPHA, 100);
	::DrawLine(BASE_X, 220, WINDOW_WIDTH - 200, 220, GetColor(255, 255, 255), 2);
	::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	auto& allBind = ConfigManager::GetIns().Bindings();

	for (int i = 0; i < GAMEPLAY_ACTION.size(); i++) {
		ActionID act = GAMEPLAY_ACTION[i];
		int y = BASE_Y + i * LINE_HEIGHT;
		int color = (i == selectNum) ? GetColor(255, 255, 0) : GetColor(255, 255, 255);

		const char* actName = TextManager::GetIns().GetActionName(act);

		// 選択カーソルとハイライト
		if (i == selectNum) {
			::DrawStringToHandle(BASE_X - 40, y, ">", color, fontMenuSmall);
			// 選択中の行の背景に薄いハイライトを入れて視認性を上げる
			::SetDrawBlendMode(DX_BLENDMODE_ALPHA, 30);
			::DrawBox(BASE_X - 50, y - 5, WINDOW_WIDTH - 200, y + LINE_HEIGHT - 5, GetColor(255, 255, 255), TRUE);
			::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		}

		// アクション名
		::DrawStringToHandle(BASE_X, y, actName ? actName : "Unknown", color, fontMenuSmall);

		int kbCode = -1;
		int padCode = -1;

		InputType kbType = InputType::KEYBOARD;
		InputType padType = InputType::JOY;

		if (allBind.count(act)) {
			for (auto& b : allBind[act]) {
				if (b.type == InputType::KEYBOARD || b.type == InputType::MOUSE) {
					kbCode = b.KeyCode;
					kbType = b.type;
				}
				if (b.type == InputType::JOY) {
					padCode = b.KeyCode;
					padType = b.type;
				}
			}
		}

		int col0Color = (i == selectNum && columnidx == 0) ? GetColor(255, 255, 0) : GetColor(255, 255, 255);
		int col1Color = (i == selectNum && columnidx == 1) ? GetColor(255, 255, 0) : GetColor(255, 255, 255);
		// キーボード列の描画
		if (isWaitingKey && i == selectNum && columnidx == 0) {
			::DrawStringToHandle(COL0_X, y, "入力待ち...", GetColor(255, 50, 50), fontDesc);
		}
		else if (kbCode != -1) {
			const char* keyName = TextManager::GetIns().GetKeyName(kbType, kbCode);
			::DrawStringToHandle(COL0_X, y, keyName, col0Color, fontMenuSmall);
		}
		else {
			::DrawStringToHandle(COL0_X, y, "NONE", GetColor(100, 100, 100), fontMenuSmall);
		}

		bool isMoveAction = (act == ActionID::MOVE_FORWARD || act == ActionID::MOVE_LEFT ||
			act == ActionID::MOVE_BACK || act == ActionID::MOVE_RIGHT);

		// コントローラー列の描画
		if (!isMoveAction) {
			if (isWaitingKey && i == selectNum && columnidx == 1) {
				::DrawStringToHandle(COL1_X, y, "入力待ち...", GetColor(255, 50, 50), fontDesc);
			}
			else if (padCode != -1) {
				const char* padName = TextManager::GetIns().GetKeyName(padType, padCode);
				::DrawStringToHandle(COL1_X, y, padName, col1Color, fontMenuSmall);
			}
			else {
				::DrawStringToHandle(COL1_X, y, "NONE", GetColor(100, 100, 100), fontMenuSmall);
			}
		}
	}
}


void TitleScene::Control() {
	int max = 0;
	switch (currentState) {
	case TitleState::TOP: max = MENU_MAX; break;
	case TitleState::MODE_SELECT: max = (int)PlayMode::MODE_MAX; break;
	case TitleState::SETTINGS: max = (int)SettingItem::MAX; break;
	case TitleState::KEY_CONFIG: max = (int)GAMEPLAY_ACTION.size(); break;
	case TitleState::CREDIT: max = 1; break;
	}

	if (!isWaitingKey) {
		int moveDir = 0;
		if (InputManager::GetIns().IsActionTrigger(ActionID::MENU_UP)) moveDir = -1;
		if (InputManager::GetIns().IsActionTrigger(ActionID::MENU_DOWN)) moveDir = 1;

		if (moveDir != 0 && max > 0) {
			SoundManager::GetIns().PlaySE("Resource/Sound/cursormove.ogg");
			do {
				selectNum += moveDir;
				if (selectNum < 0) selectNum = max - 1;
				if (selectNum >= max) selectNum = 0;

				// KEY_CONFIGステートで、コントローラー列にいる場合
				if (currentState == TitleState::KEY_CONFIG && columnidx == 1) {
					ActionID target = GAMEPLAY_ACTION[selectNum];
					bool isMoveAction = (target == ActionID::MOVE_FORWARD || target == ActionID::MOVE_LEFT ||
						target == ActionID::MOVE_BACK || target == ActionID::MOVE_RIGHT);

					// 移動アクションでなければループを抜ける（ここで停止）
					if (!isMoveAction) break;
				}
				else {
					break; // その他のステート、または左列の場合は通常通り1回の移動で抜ける
				}
			} while (true);
		}
	}
}

void TitleScene::ModifySetting(SettingItem item, int direction) {
	auto& settings = ConfigManager::GetIns().Settings();
	float dt = direction * 1.0f;
	switch (item) {
	case SettingItem::BGM_VOLUME:
		settings.bgmVolume += dt * 0.05f;
		settings.bgmVolume = std::round(settings.bgmVolume * 100.0f) / 100.0f;
		if (settings.bgmVolume < 0.00f) settings.bgmVolume = 0.00f;
		if (settings.bgmVolume > 1.00f)settings.bgmVolume = 1.00f;
		break;
	case SettingItem::SE_VOLUME:
		settings.seVolume += dt * 0.05f;
		settings.seVolume = std::round(settings.seVolume * 100.0f) / 100.0f;
		if (settings.seVolume < 0.00f) settings.seVolume = 0.00f;
		if (settings.seVolume > 1.00f)settings.seVolume = 1.00f;
		break;
	case SettingItem::MOUSE_SENSITIVITY:
		settings.mouseSensitivity += dt * 0.001f;
		settings.mouseSensitivity = std::round(settings.mouseSensitivity * 1000.0f) / 1000.0f;
		if (settings.mouseSensitivity < 0.001f) settings.mouseSensitivity = 0.001f;
		if (settings.mouseSensitivity > 0.10f) settings.mouseSensitivity = 0.10f;
		break;
	case SettingItem::PAD_SENSITIVITY:
		settings.padSensitivity += dt * 0.0001f;
		settings.padSensitivity = std::round(settings.padSensitivity * 10000.0f) / 10000.0f;
		if (settings.padSensitivity < 0.0001f)settings.padSensitivity = 0.0001f;
		if (settings.padSensitivity > 0.0100f)settings.padSensitivity = 0.0100f;
		break;
	case SettingItem::BASE_FOV:
		settings.basefov += direction * 1.0f;
		if (settings.basefov < 50.0f)settings.basefov = 50.0f;
		if (settings.basefov > 90.0f)settings.basefov = 90.0f;
		break;
	case SettingItem::HEAD_BOB:
		if (direction != 0)settings.headbob = !settings.headbob;
		break;
	case SettingItem::RECOVERY:
		if (direction != 0)settings.recovery = !settings.recovery;
		break;
	default: break;
	}
	ConfigManager::GetIns().Save();
}

void TitleScene::ChangeState(TitleState next) {
	currentState = next;
	selectNum = 0;
	columnidx = 0;
	introTimer = 0.0f;
	isWaitingKey = false;
}

void TitleScene::RemoveDuplicateBind(InputType type, int code) {
	auto& allBinding = ConfigManager::GetIns().Bindings();
	for (ActionID act : GAMEPLAY_ACTION) {
		if (allBinding.count(act)) {
			auto& vec = allBinding[act];
			//同じキーを使う操作があれば削除する
			vec.erase(std::remove_if(vec.begin(), vec.end(), [&](const KeyBind& b) {
				return b.type == type && b.KeyCode == code;
				}), vec.end());
		}
	}
}