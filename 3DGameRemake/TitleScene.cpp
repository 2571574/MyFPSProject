#include "TitleScene.h"
#include "GameScene.h"
#include <memory>
#include "InputManager.h"
TitleScene::TitleScene(SceneManager* manager) :BaseScene(manager)
,selectNum(0)
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
				selectNum = 0;
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
			manager->ChangeScene(std::make_unique<GameScene>(manager));
		}
	}
}

void TitleScene::Draw() {

}

void TitleScene::Control() {
	if (InputManager::GetIns().IsActionTrigger(ActionID::MENU_UP))
		selectNum++;
	if (InputManager::GetIns().IsActionTrigger(ActionID::MENU_DOWN))
		selectNum--;
}