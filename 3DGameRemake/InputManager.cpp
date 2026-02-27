#include "InputManager.h"

/*コンストラクタ*/
InputManager::InputManager() {
	SetDefaultBinding();
}

/*インスタンスを取得*/
InputManager& InputManager::GetIns() {
	static InputManager instance;
	return instance;
}

/*デフォルトのキーバインドにする関数*/
void InputManager::SetDefaultBinding() {
	bindings.clear();	//バインドを全消去
	//キーボードマウス
	SetBind(ActionID::MOVE_FORWARD,InputType::KEYBOARD,KEY_INPUT_W);
	SetBind(ActionID::MOVE_LEFT,InputType::KEYBOARD,KEY_INPUT_A);
	SetBind(ActionID::MOVE_BACK,InputType::KEYBOARD,KEY_INPUT_S);
	SetBind(ActionID::MOVE_RIGHT,InputType::KEYBOARD,KEY_INPUT_D);
	SetBind(ActionID::FIRE,InputType::MOUSE,MOUSE_INPUT_LEFT);
	SetBind(ActionID::ADS, InputType::MOUSE, MOUSE_INPUT_RIGHT);
	SetBind(ActionID::RELOAD, InputType::KEYBOARD, KEY_INPUT_R);
	SetBind(ActionID::ITEMTHROW, InputType::KEYBOARD, KEY_INPUT_G);
	SetBind(ActionID::RUN, InputType::KEYBOARD, KEY_INPUT_LSHIFT);
	SetBind(ActionID::JUMP, InputType::KEYBOARD, KEY_INPUT_SPACE);
	SetBind(ActionID::CROUCH, InputType::KEYBOARD, KEY_INPUT_LCONTROL);
	SetBind(ActionID::MENU_UP, InputType::KEYBOARD, KEY_INPUT_W);
	SetBind(ActionID::MENU_LEFT, InputType::KEYBOARD, KEY_INPUT_A);
	SetBind(ActionID::MENU_DOWN, InputType::KEYBOARD, KEY_INPUT_S);
	SetBind(ActionID::MENU_RIGHT, InputType::KEYBOARD, KEY_INPUT_D);
	SetBind(ActionID::MENU_SELECT, InputType::KEYBOARD, KEY_INPUT_F);
	SetBind(ActionID::MENU_BACK, InputType::MOUSE, MOUSE_INPUT_RIGHT);

	//コントローラー
	SetBind(ActionID::FIRE, InputType::JOY,PAD_INPUT_8);
	SetBind(ActionID::ADS, InputType::JOY, PAD_INPUT_7);
	SetBind(ActionID::RELOAD, InputType::JOY, PAD_INPUT_1);
	SetBind(ActionID::ITEMTHROW, InputType::JOY, PAD_INPUT_12);
	SetBind(ActionID::JUMP, InputType::JOY, PAD_INPUT_5);
	SetBind(ActionID::RUN, InputType::JOY, PAD_INPUT_2);
	SetBind(ActionID::CROUCH, InputType::JOY, PAD_INPUT_6);
	SetBind(ActionID::MENU_UP, InputType::JOY, PAD_INPUT_UP);
	SetBind(ActionID::MENU_LEFT, InputType::JOY, PAD_INPUT_LEFT);
	SetBind(ActionID::MENU_DOWN, InputType::JOY, PAD_INPUT_DOWN);
	SetBind(ActionID::MENU_RIGHT, InputType::JOY, PAD_INPUT_RIGHT);
	SetBind(ActionID::MENU_SELECT, InputType::JOY, PAD_INPUT_2);
	SetBind(ActionID::MENU_BACK, InputType::JOY, PAD_INPUT_3);
}

/*キーバインドをセット*/
void InputManager::SetBind(ActionID id, InputType type, int code) {
	bindings[id].push_back({ type,code });
}

/*ホールドでアクション*/
bool InputManager::IsActionHold(ActionID id)const {
	if (bindings.find(id) == bindings.end()) return false;	//idのアクションが見つからなければreturn
	//キー入力を検知したらtrue
	for (const auto& bind : bindings.at(id)) {
		if (CheckKey::GetIns().isHold(bind.type, bind.KeyCode)) {
			return true;
		}
	}
	return false;
}

/*トリガーでアクション*/
bool InputManager::IsActionTrigger(ActionID id)const {
	if (bindings.find(id) == bindings.end()) return false;	//idのアクションが見つからなければreturn
	//キー入力を検知したらtrue
	for (const auto& bind : bindings.at(id)) {
		if (CheckKey::GetIns().isPress(bind.type, bind.KeyCode)) {
			return true;
		}
	}
	return false;
}