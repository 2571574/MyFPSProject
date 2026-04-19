#include "TitleScene.h"
#include "TutorialScene.h"
#include "GameScene.h"
#include "InputManager.h"
#include "ConfigManager.h"
#include "TextManager.h"
#include "ResourceManager.h"
#include "SoundManager.h"
#include "Param/Global.h"
#include "Param/Scene.h"
#include "Param/System.h"
#include "Param/Visual.h"

#include <memory>
#include <algorithm>


//キーバインド可能なアクション
static const std::vector<ActionID> GAMEPLAY_ACTION = {
	ActionID::MOVE_FORWARD,ActionID::MOVE_LEFT,ActionID::MOVE_BACK,ActionID::MOVE_RIGHT,
	ActionID::FIRE,ActionID::ADS,ActionID::RELOAD,ActionID::RUN,ActionID::JUMP,ActionID::CROUCH,ActionID::WEAPON_NEXT,
	ActionID::WEAPON_PREV,ActionID::INTERACT
};


TitleScene::TitleScene(SceneManager* manager) :BaseScene(manager)
, selectNum(0), currentState(TitleState::TOP)
{}

TitleScene::~TitleScene() {}

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
	if (introTimer < Scene::Title::INTRO_DURATION) {
		introTimer += dt;
	}
	BGscrollX += Scene::Title::BG_SCROLL_X * dt;
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
				for (int i = 0; i < System::Input::MAX_KEYCODES; i++) {
					//キーボードのコードを受け取る
					if (CheckKey::GetIns().isPress(InputType::KEYBOARD, i)) {
						newCode = i;
						detectedType = InputType::KEYBOARD;
						break;
					}
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
					if (CheckKey::GetIns().isPress(InputType::MOUSE, System::Input::MOUSE_WHEEL_UP)) {
						newCode = System::Input::MOUSE_WHEEL_UP; detectedType = InputType::MOUSE;
					}
					if (CheckKey::GetIns().isPress(InputType::MOUSE, System::Input::MOUSE_WHEEL_DOWN)) {
						newCode = System::Input::MOUSE_WHEEL_DOWN; detectedType = InputType::MOUSE;
					}
					if (CheckKey::GetIns().isPress(InputType::MOUSE, MOUSE_INPUT_4)) {
						newCode = MOUSE_INPUT_4; detectedType = InputType::MOUSE;
					}
					if (CheckKey::GetIns().isPress(InputType::MOUSE, MOUSE_INPUT_5)) {
						newCode = MOUSE_INPUT_5; detectedType = InputType::MOUSE;
					}
				}
			}
			//右列はコントローラーの入力としてチェック
			else if (columnidx == 1) {
				for (int i = 1; i <= 0x8000; i <<= 1) {
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
					// コントローラー列の移動アクション選択時は何もしない
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
	if (titleBG != -1) {
		int bgWidth, bgHeight;
		GetGraphSize(titleBG, &bgWidth, &bgHeight);

		int drawOffset = (int)BGscrollX % bgWidth;
		for (int x = -drawOffset; x < System::Window::WINDOW_WIDTH; x += bgWidth) {
			::SetDrawBlendMode(DX_BLENDMODE_ALPHA, Scene::Title::BG_ALPHA_DEFAULT);
			DrawGraph(x, 0, titleBG, true);
			::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		}
		if (currentState == TitleState::SETTINGS ||
			currentState == TitleState::KEY_CONFIG ||
			currentState == TitleState::CREDIT) {
			::SetDrawBlendMode(DX_BLENDMODE_ALPHA, Scene::Title::BG_ALPHA_DARK);
			::DrawBox(0, 0, System::Window::WINDOW_WIDTH, System::Window::WINDOW_HEIGHT, ::GetColor(Global::Palette::BLACK.r, Global::Palette::BLACK.g, Global::Palette::BLACK.b), TRUE);
			::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		}
	}

	float rawProgress = introTimer / Scene::Title::INTRO_DURATION;
	float progress = (rawProgress > 1.0f) ? 1.0f : rawProgress;

	float easeOut = 1.0f - std::pow(1.0f - progress, Scene::Title::SLIDE_EASE_POWER);
	float slideX = (1.0f - easeOut) * Scene::Title::SLIDE_START_X;

	auto DrawGeometricBG = [&](int x, int y, int color, int alpha) {
		::SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
		int x1 = x;
		int y1 = y;
		int x2 = x + Scene::Title::MENU_BG_WIDTH;
		int y2 = y;
		int x3 = x + Scene::Title::MENU_BG_WIDTH + Scene::Title::MENU_BG_SLANT;
		int y3 = y + Scene::Title::MENU_BG_HEIGHT;
		int x4 = x + Scene::Title::MENU_BG_SLANT;
		int y4 = y + Scene::Title::MENU_BG_HEIGHT;

		DrawTriangle(x1, y1, x2, y2, x3, y3, color, TRUE);
		DrawTriangle(x1, y1, x3, y3, x4, y4, color, TRUE);
		::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		};

	switch (currentState) {
	case TitleState::TOP: {
		if (titleLogoHandle != -1) {
			::SetDrawBlendMode(DX_BLENDMODE_ADD, 255);
			DrawExtendGraph(System::Window::WINDOW_WIDTH - Scene::Title::TITLE_LOGO_X1, Scene::Title::TITLE_LOGO_Y1, System::Window::WINDOW_WIDTH - Scene::Title::TITLE_LOGO_X2, Scene::Title::TITLE_LOGO_Y2, titleLogoHandle, true);
			::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		}

		const char* menuLabels[MENU_MAX] = { "PLAY", "SETTINGS", "CREDIT", "EXIT" };
		for (int i = 0; i < MENU_MAX; i++) {
			int drawX = static_cast<int>(Scene::Title::MENU_BASE_X + (i * Scene::Title::MENU_SLANT_X) + slideX);
			int drawY = Scene::Title::MENU_BASE_Y + (i * Scene::Title::MENU_LINE_HEIGHT);

			bool isSelected = (i == selectNum);
			int bgColor = GetColor(Global::Palette::WHITE.r, Global::Palette::WHITE.g, Global::Palette::WHITE.b);

			int bgAlpha = isSelected ? 255 : Scene::Title::MENU_UNSELECTED_ALPHA;
			int textColor = isSelected ? GetColor(Global::Palette::BLACK.r, Global::Palette::BLACK.g, Global::Palette::BLACK.b) : GetColor(Global::Palette::WHITE.r, Global::Palette::WHITE.g, Global::Palette::WHITE.b);

			DrawGeometricBG(drawX + Scene::Title::MENU_BG_OFFSET_X, drawY + Scene::Title::MENU_BG_OFFSET_Y, bgColor, bgAlpha);

			::DrawStringToHandle(drawX, drawY, menuLabels[i], textColor, fontMenuLarge);
		}

		::SetDrawBlendMode(DX_BLENDMODE_ALPHA, Scene::Title::BG_ALPHA_DEFAULT);
		::DrawBox(Scene::Title::DIALOG_X1, Scene::Title::DIALOG_Y1, Scene::Title::DIALOG_X2, Scene::Title::DIALOG_Y2, GetColor(Global::Palette::WHITE.r, Global::Palette::WHITE.g, Global::Palette::WHITE.b), true);
		::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

		const char* description = TextManager::GetIns().GetMenuDescription(selectNum);
		::DrawStringToHandle(Scene::Title::DIALOG_X1 + Scene::Common::HELP_TEXT_MARGIN_Y, Scene::Title::DIALOG_Y1 + Scene::Common::HELP_TEXT_MARGIN_Y, description, GetColor(Global::Palette::BLACK.r, Global::Palette::BLACK.g, Global::Palette::BLACK.b), fontDesc);
		break;
	}
	case TitleState::MODE_SELECT: {
		const char* modeLabels[] = { "TUTORIAL", "EASY", "NORMAL", "HARD" };
		int modeMax = static_cast<int>(PlayMode::MODE_MAX);

		for (int i = 0; i < modeMax; i++) {
			int drawX = static_cast<int>(Scene::Title::MENU_BASE_X + (i * Scene::Title::MENU_SLANT_X) + slideX);
			int drawY = Scene::Title::MENU_BASE_Y + (i * Scene::Title::MENU_LINE_HEIGHT);

			bool isSelected = (i == selectNum);
			int bgColor = GetColor(Global::Palette::WHITE.r, Global::Palette::WHITE.g, Global::Palette::WHITE.b);

			int bgAlpha = isSelected ? 255 : Scene::Title::MENU_UNSELECTED_ALPHA;
			int textColor = isSelected ? GetColor(Global::Palette::BLACK.r, Global::Palette::BLACK.g, Global::Palette::BLACK.b) : GetColor(Global::Palette::WHITE.r, Global::Palette::WHITE.g, Global::Palette::WHITE.b);

			DrawGeometricBG(drawX + Scene::Title::MENU_BG_OFFSET_X, drawY + Scene::Title::MENU_BG_OFFSET_Y, bgColor, bgAlpha);

			::DrawStringToHandle(drawX, drawY, modeLabels[i], textColor, fontMenuLarge);
		}

		::SetDrawBlendMode(DX_BLENDMODE_ALPHA, Scene::Title::BG_ALPHA_DEFAULT);
		::DrawBox(Scene::Title::DIALOG_X1, Scene::Title::DIALOG_Y1, Scene::Title::DIALOG_X2, Scene::Title::DIALOG_Y2, GetColor(Global::Palette::WHITE.r, Global::Palette::WHITE.g, Global::Palette::WHITE.b), true);
		::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

		const char* description = TextManager::GetIns().GetMenuDescription(selectNum + 13);
		::DrawStringToHandle(Scene::Title::DIALOG_X1 + Scene::Common::HELP_TEXT_MARGIN_Y, Scene::Title::DIALOG_Y1 + Scene::Common::HELP_TEXT_MARGIN_Y, description, GetColor(Global::Palette::BLACK.r, Global::Palette::BLACK.g, Global::Palette::BLACK.b), fontDesc);
		break;
	}
	case TitleState::SETTINGS:
		DrawSettings();
		break;
	case TitleState::CREDIT: {
		int white = GetColor(Global::Palette::WHITE.r, Global::Palette::WHITE.g, Global::Palette::WHITE.b);
		int yellow = GetColor(Global::Palette::YELLOW.r, Global::Palette::YELLOW.g, Global::Palette::YELLOW.b);
		int linkColor = GetColor(Global::Palette::CYAN.r, Global::Palette::CYAN.g, Global::Palette::CYAN.b);

		::DrawStringToHandle(Scene::Title::CREDIT_BASE_X, Scene::Title::CREDIT_TITLE_Y, "CREDIT", white, fontMenuLarge);

		int currentY = Scene::Title::CREDIT_START_Y;

		::DrawStringToHandle(Scene::Title::CREDIT_BASE_X, currentY, "< モデル >", yellow, fontMenuSmall);
		currentY += Scene::Title::CREDIT_LINE_HEIGHT;
		::DrawStringToHandle(Scene::Title::CREDIT_BASE_X + Scene::Title::CREDIT_ITEM_INDENT_X, currentY, "・Quaternius", white, fontDesc);
		::DrawStringToHandle(Scene::Title::CREDIT_BASE_X + Scene::Title::CREDIT_LINK_OFFSET_X, currentY, "https://quaternius.com/", linkColor, fontDesc);
		currentY += Scene::Title::CREDIT_LINE_HEIGHT + Scene::Title::CREDIT_SECTION_GAP;

		::DrawStringToHandle(Scene::Title::CREDIT_BASE_X, currentY, "< サウンド >", yellow, fontMenuSmall);
		currentY += Scene::Title::CREDIT_LINE_HEIGHT;
		::DrawStringToHandle(Scene::Title::CREDIT_BASE_X + Scene::Title::CREDIT_ITEM_INDENT_X, currentY, "・PeriTune", white, fontDesc);
		::DrawStringToHandle(Scene::Title::CREDIT_BASE_X + Scene::Title::CREDIT_LINK_OFFSET_X, currentY, "https://peritune.com/", linkColor, fontDesc);
		currentY += Scene::Title::CREDIT_LINE_HEIGHT;
		::DrawStringToHandle(Scene::Title::CREDIT_BASE_X + Scene::Title::CREDIT_ITEM_INDENT_X, currentY, "・Pixabay", white, fontDesc);
		::DrawStringToHandle(Scene::Title::CREDIT_BASE_X + Scene::Title::CREDIT_LINK_OFFSET_X, currentY, "https://pixabay.com/", linkColor, fontDesc);
		currentY += Scene::Title::CREDIT_LINE_HEIGHT;
		::DrawStringToHandle(Scene::Title::CREDIT_BASE_X + Scene::Title::CREDIT_ITEM_INDENT_X, currentY, "・On-Jin ～音人～", white, fontDesc);
		::DrawStringToHandle(Scene::Title::CREDIT_BASE_X + Scene::Title::CREDIT_LINK_OFFSET_X, currentY, "https://on-jin.com/", linkColor, fontDesc);
		currentY += Scene::Title::CREDIT_LINE_HEIGHT;
		::DrawStringToHandle(Scene::Title::CREDIT_BASE_X + Scene::Title::CREDIT_ITEM_INDENT_X, currentY, "・Kenney", white, fontDesc);
		::DrawStringToHandle(Scene::Title::CREDIT_BASE_X + Scene::Title::CREDIT_LINK_OFFSET_X, currentY, "https://kenney.nl/", linkColor, fontDesc);
		currentY += Scene::Title::CREDIT_LINE_HEIGHT + Scene::Title::CREDIT_SECTION_GAP;

		::DrawStringToHandle(Scene::Title::CREDIT_BASE_X, currentY, "< フォント >", yellow, fontMenuSmall);
		currentY += Scene::Title::CREDIT_LINE_HEIGHT;
		::DrawStringToHandle(Scene::Title::CREDIT_BASE_X + Scene::Title::CREDIT_ITEM_INDENT_X, currentY, "・Google Fonts", white, fontDesc);
		::DrawStringToHandle(Scene::Title::CREDIT_BASE_X + Scene::Title::CREDIT_LINK_OFFSET_X, currentY, "https://fonts.google.com/", linkColor, fontDesc);
		currentY += Scene::Title::CREDIT_LINE_HEIGHT + Scene::Title::CREDIT_SECTION_GAP;

		::DrawStringToHandle(Scene::Title::CREDIT_BASE_X, currentY, "< 開発ツール・画像 >", yellow, fontMenuSmall);
		currentY += Scene::Title::CREDIT_LINE_HEIGHT;
		::DrawStringToHandle(Scene::Title::CREDIT_BASE_X + Scene::Title::CREDIT_ITEM_INDENT_X, currentY, "・Unity (マップ制作)", white, fontDesc);
		currentY += Scene::Title::CREDIT_LINE_HEIGHT;
		::DrawStringToHandle(Scene::Title::CREDIT_BASE_X + Scene::Title::CREDIT_ITEM_INDENT_X, currentY, "・Gemini (画像生成)", white, fontDesc);

		break;
	}
	case TitleState::KEY_CONFIG:
		DrawKeyConfig();
		break;
	}
	::DrawBox(0, System::Window::WINDOW_HEIGHT - Scene::Result::RESULT_HELP_BAR_HEIGHT, System::Window::WINDOW_WIDTH, System::Window::WINDOW_HEIGHT, GetColor(Global::Palette::BLACK.r, Global::Palette::BLACK.g, Global::Palette::BLACK.b), TRUE);

	std::string menuUp = TextManager::GetIns().GetActionKeyString(ActionID::MENU_UP);
	std::string menuSel = TextManager::GetIns().GetActionKeyString(ActionID::MENU_SELECT);
	std::string menuBack = TextManager::GetIns().GetActionKeyString(ActionID::MENU_BACK);
	std::string helpText = " 方向 : メニュー操作 | " + menuSel + " : 決定 | " + menuBack + " : 戻る";

	int textWidth = GetDrawStringWidthToHandle(helpText.c_str(), static_cast<int>(helpText.length()), fontDesc);
	::DrawStringToHandle(System::Window::WINDOW_WIDTH - textWidth - Scene::Common::HELP_TEXT_MARGIN_X, System::Window::WINDOW_HEIGHT - Scene::Common::HELP_TEXT_MARGIN_Y, helpText.c_str(), GetColor(Global::Palette::WHITE.r, Global::Palette::WHITE.g, Global::Palette::WHITE.b), fontDesc);
}

void TitleScene::DrawSettings() {
	::DrawStringToHandle(Scene::Title::MENU_BASE_X, Scene::Title::SETTINGS_TITLE_Y, "SETTINGS", GetColor(Global::Palette::WHITE.r, Global::Palette::WHITE.g, Global::Palette::WHITE.b), fontMenuLarge);
	auto& s = ConfigManager::GetIns().Settings();

	const char* labels[(int)SettingItem::MAX] = {
		"BGM音量", "SE音量",
		"マウス感度", "スティック感度",
		"視野角", "画面の揺れ",
		"リコイルリカバリー", "キーバインドを編集",
		"タイトルに戻る"
	};


	for (int i = 0; i < (int)SettingItem::MAX; i++) {
		int color = (i == selectNum) ? GetColor(Global::Palette::YELLOW.r, Global::Palette::YELLOW.g, Global::Palette::YELLOW.b) : GetColor(Global::Palette::WHITE.r, Global::Palette::WHITE.g, Global::Palette::WHITE.b);
		int drawY = Scene::Title::MENU_BASE_Y + i * Scene::Title::KEYCONFIG_LINE_HEIGHT;


		::DrawStringToHandle(Scene::Title::MENU_BASE_X + Scene::Title::SETTINGS_CURSOR_OFFSET_X, drawY, (i == selectNum) ? ">" : " ", color, fontMenuSmall);

		switch ((SettingItem)i) {
		case SettingItem::BGM_VOLUME:
			::DrawFormatStringToHandle(Scene::Title::MENU_BASE_X, drawY, color, fontMenuSmall, "%-20s : %d%%", labels[i], (int)(s.bgmVolume * Scene::Title::VOLUME_DISPLAY_SCALE));
			break;
		case SettingItem::SE_VOLUME:
			::DrawFormatStringToHandle(Scene::Title::MENU_BASE_X, drawY, color, fontMenuSmall, "%-20s : %d%%", labels[i], (int)(s.seVolume * Scene::Title::VOLUME_DISPLAY_SCALE));
			break;
		case SettingItem::MOUSE_SENSITIVITY:
			::DrawFormatStringToHandle(Scene::Title::MENU_BASE_X, drawY, color, fontMenuSmall, "%-20s : %.0f", labels[i], s.mouseSensitivity * Scene::Title::MOUSE_SENS_DISPLAY_SCALE);
			break;
		case SettingItem::PAD_SENSITIVITY:
			::DrawFormatStringToHandle(Scene::Title::MENU_BASE_X, drawY, color, fontMenuSmall, "%-20s : %.0f", labels[i], s.padSensitivity * Scene::Title::PAD_SENS_DISPLAY_SCALE);
			break;
		case SettingItem::BASE_FOV:
			::DrawFormatStringToHandle(Scene::Title::MENU_BASE_X, drawY, color, fontMenuSmall, "%-20s : %.0f", labels[i], s.basefov);
			break;
		case SettingItem::HEAD_BOB:
			::DrawFormatStringToHandle(Scene::Title::MENU_BASE_X, drawY, color, fontMenuSmall, "%-20s : %s", labels[i], s.headbob ? "ON" : "OFF");
			break;
		case SettingItem::RECOVERY:
			::DrawFormatStringToHandle(Scene::Title::MENU_BASE_X, drawY, color, fontMenuSmall, "%-20s : %s", labels[i], s.recovery ? "ON" : "OFF");
			break;
		case SettingItem::KEY_CONFIG:
			::DrawFormatStringToHandle(Scene::Title::MENU_BASE_X, drawY, color, fontMenuSmall, "%-20s : >>>", labels[i]);
			break;
		case SettingItem::BACK:
			::DrawFormatStringToHandle(Scene::Title::MENU_BASE_X, drawY, color, fontMenuSmall, "%s", labels[i]);
			break;
		default:
			break;
		}
	}
	::SetDrawBlendMode(DX_BLENDMODE_ALPHA, Scene::Title::BG_ALPHA_DEFAULT);
	::DrawBox(Scene::Title::DIALOG_X1, Scene::Title::DIALOG_Y1, Scene::Title::DIALOG_X2, Scene::Title::DIALOG_Y2, GetColor(Global::Palette::WHITE.r, Global::Palette::WHITE.g, Global::Palette::WHITE.b), true);
	::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	const char* description = TextManager::GetIns().GetMenuDescription(selectNum + 4);
	::DrawStringToHandle(Scene::Title::DIALOG_X1 + Scene::Common::HELP_TEXT_MARGIN_Y, Scene::Title::DIALOG_Y1 + Scene::Common::HELP_TEXT_MARGIN_Y, description, GetColor(Global::Palette::BLACK.r, Global::Palette::BLACK.g, Global::Palette::BLACK.b), fontDesc);
}

void TitleScene::DrawKeyConfig() {
	::DrawStringToHandle(Scene::Title::KEYCONFIG_TITLE_X, Scene::Title::CREDIT_TITLE_Y, "Key Config", GetColor(Global::Palette::WHITE.r, Global::Palette::WHITE.g, Global::Palette::WHITE.b), fontMenuLarge);

	::DrawStringToHandle(Scene::Title::KEYCONFIG_COL_KB_X, Scene::Title::KEYCONFIG_HEADER_Y, "キーボード/マウス", GetColor(Global::Palette::GRAY.r, Global::Palette::GRAY.g, Global::Palette::GRAY.b), fontMenuSmall);
	::DrawStringToHandle(Scene::Title::KEYCONFIG_COL_PAD_X, Scene::Title::KEYCONFIG_HEADER_Y, "コントローラー", GetColor(Global::Palette::GRAY.r, Global::Palette::GRAY.g, Global::Palette::GRAY.b), fontMenuSmall);

	::SetDrawBlendMode(DX_BLENDMODE_ALPHA, Scene::Title::BG_ALPHA_DEFAULT);
	::DrawLine(Scene::Title::KEYCONFIG_BASE_X, Scene::Title::KEYCONFIG_LINE_Y, 
		System::Window::WINDOW_WIDTH - Scene::Title::KEYCONFIG_LINE_MARGIN_X, Scene::Title::KEYCONFIG_LINE_Y, 
		GetColor(Global::Palette::WHITE.r, Global::Palette::WHITE.g, Global::Palette::WHITE.b), Scene::Title::KEYCONFIG_LINE_THICKNESS);
	::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	auto& allBind = ConfigManager::GetIns().Bindings();

	for (int i = 0; i < GAMEPLAY_ACTION.size(); i++) {
		ActionID act = GAMEPLAY_ACTION[i];
		int y = Scene::Title::KEYCONFIG_BASE_Y + i * Scene::Title::KEYCONFIG_LINE_HEIGHT;
		int color = (i == selectNum) ? GetColor(Global::Palette::YELLOW.r, Global::Palette::YELLOW.g, Global::Palette::YELLOW.b) : GetColor(Global::Palette::WHITE.r, Global::Palette::WHITE.g, Global::Palette::WHITE.b);

		const char* actName = TextManager::GetIns().GetActionName(act);

		if (i == selectNum) {
			::DrawStringToHandle(Scene::Title::KEYCONFIG_BASE_X + Scene::Title::KEYCONFIG_CURSOR_OFFSET_X, y, ">", color, fontMenuSmall);
			::SetDrawBlendMode(DX_BLENDMODE_ALPHA, Scene::Title::KEYCONFIG_HIGHLIGHT_ALPHA);
			::DrawBox(Scene::Title::KEYCONFIG_BASE_X + Scene::Title::KEYCONFIG_BG_OFFSET_X,
				y + Scene::Title::KEYCONFIG_BG_OFFSET_Y,
				System::Window::WINDOW_WIDTH - Scene::Title::KEYCONFIG_LINE_MARGIN_X,
				y + Scene::Title::KEYCONFIG_LINE_HEIGHT + Scene::Title::KEYCONFIG_BG_OFFSET_Y,
				GetColor(Global::Palette::WHITE.r, Global::Palette::WHITE.g, Global::Palette::WHITE.b), TRUE);
			::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		}

		::DrawStringToHandle(Scene::Title::KEYCONFIG_BASE_X, y, actName ? actName : "Unknown", color, fontMenuSmall);

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

		int col0Color = (i == selectNum && columnidx == 0) ? GetColor(Global::Palette::YELLOW.r, Global::Palette::YELLOW.g, Global::Palette::YELLOW.b) : GetColor(Global::Palette::WHITE.r, Global::Palette::WHITE.g, Global::Palette::WHITE.b);
		int col1Color = (i == selectNum && columnidx == 1) ? GetColor(Global::Palette::YELLOW.r, Global::Palette::YELLOW.g, Global::Palette::YELLOW.b) : GetColor(Global::Palette::WHITE.r, Global::Palette::WHITE.g, Global::Palette::WHITE.b);

		if (isWaitingKey && i == selectNum && columnidx == 0) {
			::DrawStringToHandle(Scene::Title::KEYCONFIG_COL_KB_X, y, "入力待ち...", GetColor(Global::Palette::RED_LIGHT.r, Global::Palette::RED_LIGHT.g, Global::Palette::RED_LIGHT.b), fontDesc);
		}
		else if (kbCode != -1) {
			const char* keyName = TextManager::GetIns().GetKeyName(kbType, kbCode);
			::DrawStringToHandle(Scene::Title::KEYCONFIG_COL_KB_X, y, keyName, col0Color, fontMenuSmall);
		}
		else {
			::DrawStringToHandle(Scene::Title::KEYCONFIG_COL_KB_X, y, "NONE", GetColor(Global::Palette::GRAY.r, Global::Palette::GRAY.g, Global::Palette::GRAY.b), fontMenuSmall);
		}

		bool isMoveAction = (act == ActionID::MOVE_FORWARD || act == ActionID::MOVE_LEFT ||
			act == ActionID::MOVE_BACK || act == ActionID::MOVE_RIGHT);

		if (!isMoveAction) {
			if (isWaitingKey && i == selectNum && columnidx == 1) {
				::DrawStringToHandle(Scene::Title::KEYCONFIG_COL_PAD_X, y, "入力待ち...", GetColor(Global::Palette::RED_LIGHT.r, Global::Palette::RED_LIGHT.g, Global::Palette::RED_LIGHT.b), fontDesc);
			}
			else if (padCode != -1) {
				const char* padName = TextManager::GetIns().GetKeyName(padType, padCode);
				::DrawStringToHandle(Scene::Title::KEYCONFIG_COL_PAD_X, y, padName, col1Color, fontMenuSmall);
			}
			else {
				::DrawStringToHandle(Scene::Title::KEYCONFIG_COL_PAD_X, y, "NONE", GetColor(Global::Palette::GRAY.r, Global::Palette::GRAY.g, Global::Palette::GRAY.b), fontMenuSmall);
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

				if (currentState == TitleState::KEY_CONFIG && columnidx == 1) {
					ActionID target = GAMEPLAY_ACTION[selectNum];
					bool isMoveAction = (target == ActionID::MOVE_FORWARD || target == ActionID::MOVE_LEFT ||
						target == ActionID::MOVE_BACK || target == ActionID::MOVE_RIGHT);

					if (!isMoveAction) break;
				}
				else {
					break;
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
		settings.bgmVolume += dt * Scene::Title::VOLUME_STEP;
		settings.bgmVolume = std::round(settings.bgmVolume * 100.0f) / 100.0f;
		if (settings.bgmVolume < 0.00f) settings.bgmVolume = 0.00f;
		if (settings.bgmVolume > 1.00f)settings.bgmVolume = 1.00f;
		break;
	case SettingItem::SE_VOLUME:
		settings.seVolume += dt * Scene::Title::VOLUME_STEP;
		settings.seVolume = std::round(settings.seVolume * 100.0f) / 100.0f;
		if (settings.seVolume < 0.00f) settings.seVolume = 0.00f;
		if (settings.seVolume > 1.00f)settings.seVolume = 1.00f;
		break;
	case SettingItem::MOUSE_SENSITIVITY:
		settings.mouseSensitivity += dt * Scene::Title::MOUSE_SENS_STEP;
		settings.mouseSensitivity = std::round(settings.mouseSensitivity * 1000.0f) / 1000.0f;
		if (settings.mouseSensitivity < 0.001f) settings.mouseSensitivity = 0.001f;
		if (settings.mouseSensitivity > 0.10f) settings.mouseSensitivity = 0.10f;
		break;
	case SettingItem::PAD_SENSITIVITY:
		settings.padSensitivity += dt * Scene::Title::PAD_SENS_STEP;
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
			vec.erase(std::remove_if(vec.begin(), vec.end(), [&](const KeyBind& b) {
				return b.type == type && b.KeyCode == code;
				}), vec.end());
		}
	}
}