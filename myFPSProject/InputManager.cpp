#include "InputManager.h"
#include "Param/System.h"
#include "DxLib.h"

InputManager::InputManager() {
	SetDefaultBinding();
}

InputManager& InputManager::GetIns() {
	static InputManager instance;
	return instance;
}

void InputManager::SetDefaultBinding() {
	bindings.clear();

	//キーボードマウス
	SetBind(ActionID::MOVE_FORWARD, InputType::KEYBOARD, KEY_INPUT_W);
	SetBind(ActionID::MOVE_LEFT, InputType::KEYBOARD, KEY_INPUT_A);
	SetBind(ActionID::MOVE_BACK, InputType::KEYBOARD, KEY_INPUT_S);
	SetBind(ActionID::MOVE_RIGHT, InputType::KEYBOARD, KEY_INPUT_D);
	SetBind(ActionID::FIRE, InputType::MOUSE, MOUSE_INPUT_LEFT);
	SetBind(ActionID::ADS, InputType::MOUSE, MOUSE_INPUT_RIGHT);
	SetBind(ActionID::RELOAD, InputType::KEYBOARD, KEY_INPUT_R);
	SetBind(ActionID::RUN, InputType::KEYBOARD, KEY_INPUT_LSHIFT);
	SetBind(ActionID::JUMP, InputType::KEYBOARD, KEY_INPUT_SPACE);
	SetBind(ActionID::CROUCH, InputType::KEYBOARD, KEY_INPUT_LCONTROL);
	SetBind(ActionID::WEAPON_NEXT, InputType::MOUSE, System::Input::MOUSE_WHEEL_DOWN);
	SetBind(ActionID::WEAPON_PREV, InputType::MOUSE, System::Input::MOUSE_WHEEL_UP);
	SetBind(ActionID::INTERACT, InputType::KEYBOARD, KEY_INPUT_F);
	SetBind(ActionID::MENU_UP, InputType::KEYBOARD, KEY_INPUT_W);
	SetBind(ActionID::MENU_LEFT, InputType::KEYBOARD, KEY_INPUT_A);
	SetBind(ActionID::MENU_DOWN, InputType::KEYBOARD, KEY_INPUT_S);
	SetBind(ActionID::MENU_RIGHT, InputType::KEYBOARD, KEY_INPUT_D);
	SetBind(ActionID::MENU_SELECT, InputType::KEYBOARD, KEY_INPUT_F);
	SetBind(ActionID::MENU_BACK, InputType::KEYBOARD, KEY_INPUT_LSHIFT);
	SetBind(ActionID::PAUSE, InputType::KEYBOARD, KEY_INPUT_ESCAPE);

	//コントローラー
	SetBind(ActionID::FIRE, InputType::JOY, PAD_INPUT_8); 
	SetBind(ActionID::ADS, InputType::JOY, PAD_INPUT_7);  
	SetBind(ActionID::RELOAD, InputType::JOY, PAD_INPUT_1);  
	SetBind(ActionID::JUMP, InputType::JOY, PAD_INPUT_2);  
	SetBind(ActionID::RUN, InputType::JOY, PAD_INPUT_6); 
	SetBind(ActionID::CROUCH, InputType::JOY, PAD_INPUT_3); 
	SetBind(ActionID::WEAPON_NEXT, InputType::JOY, PAD_INPUT_RIGHT); 
	SetBind(ActionID::WEAPON_PREV, InputType::JOY, PAD_INPUT_LEFT);
	SetBind(ActionID::INTERACT, InputType::JOY, PAD_INPUT_4); 
	SetBind(ActionID::PAUSE, InputType::JOY, PAD_INPUT_10);

	SetBind(ActionID::MENU_UP, InputType::JOY, PAD_INPUT_UP);
	SetBind(ActionID::MENU_LEFT, InputType::JOY, PAD_INPUT_LEFT);
	SetBind(ActionID::MENU_DOWN, InputType::JOY, PAD_INPUT_DOWN);
	SetBind(ActionID::MENU_RIGHT, InputType::JOY, PAD_INPUT_RIGHT);
	SetBind(ActionID::MENU_SELECT, InputType::JOY, PAD_INPUT_2);
	SetBind(ActionID::MENU_BACK, InputType::JOY, PAD_INPUT_3);
}


void InputManager::SetBind(ActionID id, InputType type, int code) {
	bindings[id].push_back({ type,code });
}


bool InputManager::IsActionHold(ActionID id)const {
	auto it = bindings.find(id);
	if (it == bindings.end()) return false;

	for (const auto& bind : it->second) {
		if (CheckKey::GetIns().isHold(bind.type, bind.KeyCode)) {
			return true;
		}
	}
	return false;
}


bool InputManager::IsActionTrigger(ActionID id)const {
	auto it = bindings.find(id);
	if (it == bindings.end()) return false;

	for (const auto& bind : it->second) {
		if (CheckKey::GetIns().isPress(bind.type, bind.KeyCode)) {
			return true;
		}
	}
	return false;
}