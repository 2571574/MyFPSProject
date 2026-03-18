#include "TitleScene.h"
#include "TutorialScene.h"
#include "GameScene.h"
#include "InputManager.h"
#include "ConfigManager.h"
#include "TextManager.h"
#include "ResourceManager.h"
#include "Debug.h"

#include <memory>
#include<algorithm>

namespace {
	constexpr int KEY_MAX = 256;
	constexpr int BUTTON_MAX = 0x8000;
	constexpr float BG_SCROLL_X = 50.0f;
	constexpr float BOBSPEED = 10.0f;

	constexpr int DIALOG_X1 = CENTER_X + 50;
	constexpr int DIALOG_Y1 = CENTER_Y + 100;
	constexpr int DIALOG_X2 = WINDOW_WIDTH - 50;
	constexpr int DIALOG_Y2 = WINDOW_HEIGHT - 50;
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
	ChangeState(TitleState::TOP);
}


void TitleScene::Update() {
	Control();
	float dt = Time::GetIns().GetDelta();
	BGscrollX += BG_SCROLL_X * dt;
	if (selectNum < 0) selectNum = 0;
	switch (currentState) {
	case TitleState::TOP:

		if (selectNum >= MENU_MAX) {
			selectNum = MENU_MAX - 1;
		}
		if (InputManager::GetIns().IsActionTrigger(ActionID::MENU_SELECT)) {
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
			ChangeState(TitleState::TOP);
		}
		if (InputManager::GetIns().IsActionTrigger(ActionID::MENU_SELECT)) {
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
			ModifySetting((SettingItem)selectNum, -1);
		}
		if (InputManager::GetIns().IsActionTrigger(ActionID::MENU_RIGHT)) {
			ModifySetting((SettingItem)selectNum, 1);
		}
		if (InputManager::GetIns().IsActionTrigger(ActionID::MENU_BACK)) {
			ChangeState(TitleState::TOP);
		}
		if (InputManager::GetIns().IsActionTrigger(ActionID::MENU_SELECT)) {
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
			if (InputManager::GetIns().IsActionTrigger(ActionID::MENU_LEFT))columnidx = 0;
			if (InputManager::GetIns().IsActionTrigger(ActionID::MENU_RIGHT))columnidx = 1;
			if (InputManager::GetIns().IsActionTrigger(ActionID::MENU_BACK)) {
				ChangeState(TitleState::SETTINGS);
				selectNum = (int)SettingItem::KEY_CONFIG;
			}
			if (InputManager::GetIns().IsActionTrigger(ActionID::MENU_SELECT)) {
				isWaitingKey = true;
			}
		}
		break;

	case TitleState::CREDIT:
		if (InputManager::GetIns().IsActionTrigger(ActionID::MENU_BACK)) {
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
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 100);
			DrawGraph(x, 0, titleBG, true);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		}
	}

	switch (currentState) {
	case TitleState::TOP: {

		if (titleLogoHandle != -1) {
			int margin = 100;
			int x2 = WINDOW_WIDTH - 300;
			int x1 = x2 - 500;
			int y1 = margin;
			int y2 = y1 + 500;
			SetDrawBlendMode(DX_BLENDMODE_ADD, 255);
			DrawExtendGraph(x1, y1, x2, y2, titleLogoHandle, true);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		}
		DrawString(CHOOSE_X, BASE_Y + LINE_HEIGHT * selectNum, ">", GetColor(255, 255, 255));
		DrawString(BASE_X, BASE_Y, "Play", GetColor(255, 255, 255));
		DrawString(BASE_X, BASE_Y + LINE_HEIGHT, "Settings", GetColor(255, 255, 255));
		DrawString(BASE_X, BASE_Y + LINE_HEIGHT * 2, "Credit", GetColor(255, 255, 255));
		DrawString(BASE_X, BASE_Y + LINE_HEIGHT * 3, "Exit", GetColor(255, 255, 255));
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 100);
		DrawBox(DIALOG_X1, DIALOG_Y1, DIALOG_X2, DIALOG_Y2, GetColor(255, 255, 255), true);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

		const char* description = TextManager::GetIns().GetMenuDescription(selectNum);
		DrawString(DIALOG_X1 + 30, DIALOG_Y1 + 30, description, GetColor(0, 0, 0));
		break;
	}
	case TitleState::MODE_SELECT: {
		DrawString(CHOOSE_X, 100 + 30 * selectNum, ">", GetColor(255, 255, 255));
		DrawString(BASE_X, BASE_Y, "Tutorial", GetColor(255, 255, 255));
		DrawString(BASE_X, BASE_Y + LINE_HEIGHT, "Easy", GetColor(255, 255, 255));
		DrawString(BASE_X, BASE_Y + LINE_HEIGHT * 2, "Normal", GetColor(255, 255, 255));
		DrawString(BASE_X, BASE_Y + LINE_HEIGHT * 3, "Hard", GetColor(255, 255, 255));
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 100);
		DrawBox(DIALOG_X1, DIALOG_Y1, DIALOG_X2, DIALOG_Y2, GetColor(255, 255, 255), true);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

		const char* description = TextManager::GetIns().GetMenuDescription(selectNum + 13);
		DrawString(DIALOG_X1 + 30, DIALOG_Y1 + 30, description, GetColor(0, 0, 0));
		break;
	}
	case TitleState::SETTINGS:
		DrawSettings();
		break;
	case TitleState::CREDIT:
		DrawString(100, 100, "Credit", GetColor(255, 255, 255));
		break;
	case TitleState::KEY_CONFIG:
		DrawKeyConfig();
		break;

	}
	
	DrawFormatString(WINDOW_WIDTH - 600, WINDOW_HEIGHT - 30, GetColor(255, 255, 255), "WASD : メニュー操作 , F : 決定, SHIFT : 戻る");
}

void TitleScene::DrawSettings() {
	const int BASE_X = 100;
	const int CHOOSE_X = BASE_X - 20;
	const int BASE_Y = 100;
	const int LINE_HEIGHT = 30;

	DrawString(BASE_X, 50, "Settings", GetColor(255, 255, 255));
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

		
		DrawString(CHOOSE_X, drawY, (i == selectNum) ? ">" : " ", color);

		//描画
		switch ((SettingItem)i) {
		case SettingItem::BGM_VOLUME:
			DrawFormatString(BASE_X, drawY, color, "%-20s : %d%%", labels[i], (int)(s.bgmVolume * 100));
			break;
		case SettingItem::SE_VOLUME:
			DrawFormatString(BASE_X, drawY, color, "%-20s : %d%%", labels[i], (int)(s.seVolume * 100));
			break;
		case SettingItem::MOUSE_SENSITIVITY:
			DrawFormatString(BASE_X, drawY, color, "%-20s : %.1f", labels[i], s.mouseSensitivity * 2.0f);
			break;
		case SettingItem::PAD_SENSITIVITY:
			DrawFormatString(BASE_X, drawY, color, "%-20s : %.1f", labels[i], s.padSensitivity * 1000.0f);
			break;
		case SettingItem::BASE_FOV:
			DrawFormatString(BASE_X, drawY, color, "%-20s : %.0f", labels[i], s.basefov);
			break;
		case SettingItem::HEAD_BOB:
			DrawFormatString(BASE_X, drawY, color, "%-20s : %s", labels[i], s.headbob ? "ON" : "OFF");
			break;
		case SettingItem::RECOVERY:
			DrawFormatString(BASE_X, drawY, color, "%-20s : %s", labels[i], s.recovery ? "ON" : "OFF");
			break;
		case SettingItem::KEY_CONFIG:
			DrawFormatString(BASE_X, drawY, color, "%-20s : >>>", labels[i]);
			break;
		case SettingItem::BACK:
			DrawFormatString(BASE_X, drawY, color, "%s", labels[i]);
			break;
		default:
			break;
		}
	}
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 100);
	DrawBox(DIALOG_X1, DIALOG_Y1, DIALOG_X2, DIALOG_Y2, GetColor(255, 255, 255), true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	const char* description = TextManager::GetIns().GetMenuDescription(selectNum + 4);
	DrawString(DIALOG_X1 + 30, DIALOG_Y1 + 30, description, GetColor(0, 0, 0));
}

void TitleScene::DrawKeyConfig() {
	const int BASE_X = 100;
	const int BASE_Y = 100;
	const int LINE_HEIGHT = 25;
	const int COL0_X = 300; 
	const int COL1_X = 480;

	DrawString(BASE_X, 30, "---Key Config---", GetColor(255, 255, 255));
	DrawString(BASE_X, 60, "          キーボード/マウス      コントローラー", GetColor(200, 200, 200));

	auto& allBind = ConfigManager::GetIns().Bindings();

	for (int i = 0; i < GAMEPLAY_ACTION.size(); i++) {
		ActionID act = GAMEPLAY_ACTION[i];
		int y = BASE_Y + i * LINE_HEIGHT;
		int color = (i == selectNum) ? GetColor(255, 255, 0) : GetColor(255, 255, 255);

		const char* actName = TextManager::GetIns().GetActionName(act);

		
		DrawString(BASE_X - 20, y, (i == selectNum) ? ">" : " ", color);
		DrawString(BASE_X, y, actName ? actName : "Unknown", color);

		int kbCode = -1;
		int padCode = -1;

		if (allBind.count(act)) {
			for (auto& b : allBind[act]) {
				if (b.type == InputType::KEYBOARD || b.type == InputType::MOUSE) kbCode = b.KeyCode;
				if (b.type == InputType::JOY) padCode = b.KeyCode;
			}
		}

		
		int col0Color = (i == selectNum && columnidx == 0) ? GetColor(255, 100, 100) : color;
		if (isWaitingKey && i == selectNum && columnidx == 0) {
			DrawString(COL0_X, y, "キー入力待ち", GetColor(255, 50, 50));
		}
		else if (kbCode != -1) {
			DrawFormatString(COL0_X, y, col0Color, "%d", kbCode);
		}
		else {
			DrawString(COL0_X, y, "NONE", col0Color);
		}

		
		int col1Color = (i == selectNum && columnidx == 1) ? GetColor(255, 100, 100) : color;
		if (isWaitingKey && i == selectNum && columnidx == 1) {
			DrawString(COL1_X, y, "ボタン入力待ち", GetColor(255, 50, 50));
		}
		else if (padCode != -1) {
			DrawFormatString(COL1_X, y, col1Color, "%d", padCode);
		}
		else {
			DrawString(COL1_X, y, "NONE", col1Color);
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
		if (InputManager::GetIns().IsActionTrigger(ActionID::MENU_UP))
			selectNum--;
		if (InputManager::GetIns().IsActionTrigger(ActionID::MENU_DOWN))
			selectNum++;
		if (max > 0) {
			if (selectNum < 0)selectNum = max - 1;
			if (selectNum >= max)selectNum = 0;
		}
	}
}

void TitleScene::ModifySetting(SettingItem item, int direction) {
	auto& settings = ConfigManager::GetIns().Settings();
	float dt = direction * 1.0f;
	switch (item) {
	case SettingItem::BGM_VOLUME:
		settings.bgmVolume += dt * 0.05f;
		if (settings.bgmVolume < 0.00f) settings.bgmVolume = 0.00f;
		if (settings.bgmVolume > 1.00f)settings.bgmVolume = 1.00f;
		break;
	case SettingItem::SE_VOLUME:
		settings.seVolume += dt * 0.05f;
		if (settings.seVolume < 0.00f) settings.seVolume = 0.00f;
		if (settings.seVolume > 1.00f)settings.seVolume = 1.00f;
		break;
	case SettingItem::MOUSE_SENSITIVITY:
		settings.mouseSensitivity += dt * 0.05f;
		if (settings.mouseSensitivity < 0.05f) settings.mouseSensitivity = 0.05f;
		if (settings.mouseSensitivity > 1.0f) settings.mouseSensitivity = 1.0f;
		break;
	case SettingItem::PAD_SENSITIVITY:
		settings.padSensitivity += dt * 0.0001f;
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