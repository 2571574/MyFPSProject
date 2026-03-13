#include "TitleScene.h"
#include "GameScene.h"
#include <memory>
#include "InputManager.h"
TitleScene::TitleScene(SceneManager* manager) :BaseScene(manager)
, selectNum(0), currentState(TitleState::TOP)
{}

TitleScene::~TitleScene() {

}

void TitleScene::Init() {
	selectNum = 0;
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
				currentState = TitleState::MODE_SELECT;
				selectNum = MODE_TUTORIAL;
			}
			else if (selectNum == MENU_SETTING) {
				currentState = TitleState::SETTINGS;
				selectNum = 0;
			}
			else if (selectNum == MENU_CREDIT) {
				currentState = TitleState::CREDIT;
				selectNum = 0;
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
			currentState = TitleState::TOP;
			selectNum = MENU_PLAY;
		}
		if (InputManager::GetIns().IsActionTrigger(ActionID::MENU_SELECT)) {
			manager->SetPlayMode((PlayMode)selectNum);
			manager->ChangeScene(std::make_unique<GameScene>(manager));
		}
		break;

	case TitleState::SETTINGS:
		if (InputManager::GetIns().IsActionTrigger(ActionID::MENU_BACK)) {
			currentState = TitleState::TOP;
			selectNum = MENU_PLAY;
		}
		if (InputManager::GetIns().IsActionTrigger(ActionID::MENU_SELECT)) {
			
		}
		break;

	case TitleState::CREDIT:
		if (InputManager::GetIns().IsActionTrigger(ActionID::MENU_BACK)) {
			currentState = TitleState::TOP;
			selectNum = MENU_PLAY;
		}
		if (InputManager::GetIns().IsActionTrigger(ActionID::MENU_SELECT)) {
		}
		break;

	case TitleState::KEY_CONFIG:
		if (InputManager::GetIns().IsActionTrigger(ActionID::MENU_BACK)) {
			currentState = TitleState::SETTINGS;
			selectNum = MENU_PLAY;
		}
		if (InputManager::GetIns().IsActionTrigger(ActionID::MENU_SELECT)) {
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
		DrawString(100, 100, "Settings", GetColor(255, 255, 255));
		break;
	case TitleState::CREDIT:
		DrawString(100, 100, "Credit", GetColor(255, 255, 255));
		break;
	case TitleState::KEY_CONFIG:
		DrawString(100, 100, "Key Config", GetColor(255, 255, 255));
		break;
		
	}
}

void TitleScene::Control() {
	if (InputManager::GetIns().IsActionTrigger(ActionID::MENU_UP))
		selectNum--;
	if (InputManager::GetIns().IsActionTrigger(ActionID::MENU_DOWN))
		selectNum++;
}