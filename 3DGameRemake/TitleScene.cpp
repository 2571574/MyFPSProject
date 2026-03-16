#include "TitleScene.h"
#include "TutorialScene.h"
#include "GameScene.h"
#include "InputManager.h"
#include "ConfigManager.h"
#include "TextManager.h"
#include <memory>
#include<algorithm>
#include <cstdio>

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
	ChangeState(TitleState::TOP);
}

void TitleScene::Update() {
	Control();
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
		if (selectNum >= MODE_MAX) {
			selectNum = MODE_MAX - 1;
		}
		if (InputManager::GetIns().IsActionTrigger(ActionID::MENU_BACK)) {
			ChangeState(TitleState::TOP);
		}
		if (InputManager::GetIns().IsActionTrigger(ActionID::MENU_SELECT)) {
			manager->SetPlayMode((PlayMode)selectNum);
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
		if (isWaitingKey) {
			int newCode = -1;
			InputType detectedType = InputType::KEYBOARD;
			if (columnidx == 0) {
				for (int i = 0; i < 256; i++) {
					if (CheckKey::GetIns().isHold(InputType::KEYBOARD,i)) {
						newCode = i;
						detectedType = InputType::KEYBOARD;
						break;
					}
					if (newCode == -1) {
						if (CheckKey::GetIns().isHold(InputType::MOUSE, MOUSE_INPUT_LEFT)) {
							newCode = MOUSE_INPUT_LEFT; detectedType = InputType::MOUSE;
						}
						if (CheckKey::GetIns().isHold(InputType::MOUSE, MOUSE_INPUT_RIGHT)) {
							newCode = MOUSE_INPUT_RIGHT; detectedType = InputType::MOUSE;
						}
						if (CheckKey::GetIns().isHold(InputType::MOUSE, MOUSE_INPUT_MIDDLE)) {
							newCode = MOUSE_INPUT_MIDDLE; detectedType = InputType::MOUSE;
						}
						if (CheckKey::GetIns().isHold(InputType::MOUSE, MOUSE_WHEEL_UP)) {
							newCode = MOUSE_WHEEL_UP; detectedType = InputType::MOUSE;
						}
						if (CheckKey::GetIns().isHold(InputType::MOUSE, MOUSE_WHEEL_DOWN)) {
							newCode = MOUSE_WHEEL_DOWN; detectedType = InputType::MOUSE;
						}
						if (CheckKey::GetIns().isHold(InputType::MOUSE, MOUSE_INPUT_4)) {
							newCode = MOUSE_INPUT_4; detectedType = InputType::MOUSE;
						}
						if (CheckKey::GetIns().isHold(InputType::MOUSE, MOUSE_INPUT_5)) {
							newCode = MOUSE_INPUT_5; detectedType = InputType::MOUSE;
						}
					}
				}
			}
			else if (columnidx == -1) {
				for (int i = 1; i <= 0x8000; i <<= 1) {
					if (CheckKey::GetIns().isHold(InputType::JOY, i)) { 
						newCode = i; 
						detectedType = InputType::JOY; 
						break;
					}
				}
			}

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
	switch (currentState) {
	case TitleState::TOP:

		DrawString(80, 100 + 30 * selectNum, ">", GetColor(255, 255, 255));
		DrawString(100, 100, "Play", GetColor(255, 255, 255));
		DrawString(100, 130, "Settings", GetColor(255, 255, 255));
		DrawString(100, 160, "Credit", GetColor(255, 255, 255));
		DrawString(100, 190, "Exit", GetColor(255, 255, 255));
		break;
	case TitleState::MODE_SELECT:
		DrawString(80, 100 + 30 * selectNum, ">", GetColor(255, 255, 255));
		DrawString(100, 100, "Tutorial", GetColor(255, 255, 255));
		DrawString(100, 130, "Easy", GetColor(255, 255, 255));
		DrawString(100, 160, "Normal", GetColor(255, 255, 255));
		DrawString(100, 190, "Hard", GetColor(255, 255, 255));
		break;
	case TitleState::SETTINGS:
		DrawSettings();
		break;
	case TitleState::CREDIT:
		DrawKeyConfig();
		DrawString(100, 100, "Credit", GetColor(255, 255, 255));
		break;
	case TitleState::KEY_CONFIG:
		DrawKeyConfig();
		break;
		
	}
}

void TitleScene::DrawSettings() {
	DrawString(100, 50, "Settings", GetColor(255, 255, 255));
	auto& s = ConfigManager::GetIns().Settings();

	const char* labels[] = {
		"マウス感度","スティック感度",
		"視野角","画面の揺れ",
		"リコイルリカバリー","キーバインドを編集",
		"タイトルに戻る"
	};

	char valStr[7][64];
	sprintf_s(valStr[0], "%.2f", s.mouseSensitivity);
	sprintf_s(valStr[1], "%.3f", s.padSensitivity);
	sprintf_s(valStr[2], "%.0f", s.basefov);
	sprintf_s(valStr[3], s.headbob ?  "ON": "OFF");
	sprintf_s(valStr[4], s.recovery ?  "ON": "OFF");
	sprintf_s(valStr[5], ">>>");
	sprintf_s(valStr[6], "");
	
	for (int i = 0; i < (int)SettingItem::MAX; i++) {
		int color = (i == selectNum) ? GetColor(255, 255, 0) : GetColor(255, 255, 255);
		DrawString(80, 100 + i * 30, (i == selectNum) ? ">" : " ", color);
		DrawFormatString(100, 100 + i * 30, color, "%-20s : %s", labels[i], valStr[i]);
	}
}

void TitleScene::DrawKeyConfig() {
	DrawString(100, 30, "---Key Config---", GetColor(255, 255, 255));
	DrawString(100, 60, "          キーボード/マウス      コントローラー",GetColor(200,200,200));

	auto& allBind = ConfigManager::GetIns().Bindings();

	for (int i = 0; i < GAMEPLAY_ACTION.size(); i++) {
		ActionID act = GAMEPLAY_ACTION[i];
		int y = 100 + i * 25;
		int color = (i == selectNum) ? GetColor(255, 255, 0) : GetColor(255, 255, 255);

		const char* actName = TextManager::GetIns().GetActionName(act);
		DrawString(80, y, (i == selectNum) ? ">" : " ", color);
		DrawString(100, y, actName ? actName : "Unknown", color);

		int kbCode = -1;
		int padCode = -1;

		if (allBind.count(act)) {
			for (auto& b : allBind[act]) {
				if (b.type == InputType::KEYBOARD || b.type == InputType::MOUSE)kbCode = b.KeyCode;
				if (b.type == InputType::JOY)padCode = b.KeyCode;
			}
		}

		int col0Color = (i == selectNum && columnidx == 0) ? GetColor(255, 100, 100) : color;
		if (isWaitingKey && i == selectNum && columnidx == 0) DrawString(300, y, "キー入力待ち", GetColor(255, 50, 50));
		else if (kbCode != -1)DrawFormatString(300, y, col0Color, "%d", kbCode);

		else DrawString(300, y, "NONE", col0Color);


		int col1Color = (i == selectNum && columnidx == 1) ? GetColor(255, 100, 100) : color;
		if (isWaitingKey && i == selectNum && columnidx == 1) DrawString(480, y, "ボタン入力待ち", GetColor(255, 50, 50));
		else if (padCode != -1)DrawFormatString(480, y, col1Color, "%d", padCode);

		else DrawString(480, y, "NONE", col1Color);
	}

}

void TitleScene::Control() {
	int max = 0;
	switch (currentState) {
	case TitleState::TOP: max = MENU_MAX; break;
	case TitleState::MODE_SELECT: max = MODE_MAX; break;
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
	float dt = direction * 0.1f;
	switch (item) {
	case SettingItem::MOUSE_SENSITIVITY:
		settings.mouseSensitivity += dt * 0.5f;
		if (settings.mouseSensitivity < 0.01f) settings.mouseSensitivity = 0.01f;
		if (settings.mouseSensitivity > 5.0f) settings.mouseSensitivity = 5.0f;
		break;
	case SettingItem::PAD_SENSITIVITY:
		settings.padSensitivity += dt * 0.001f;
		if (settings.padSensitivity < 0.001f)settings.padSensitivity = 0.001f;
		if (settings.padSensitivity > 2.0f)settings.padSensitivity = 2.0f;
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
	for (auto& pair : allBinding) {
		auto& vec = pair.second;
		vec.erase(std::remove_if(vec.begin(), vec.end(), [&](const KeyBind& b) {
			return b.type == type && b.KeyCode == code;
			}), vec.end());
	}
}