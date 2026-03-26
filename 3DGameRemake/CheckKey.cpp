#include "CheckKey.h"
#include "DxLib.h"
#include <cstring>

/*コンストラクタ*/
CheckKey::CheckKey() {
	//コントローラーのデッドゾーンを設定
	SetJoypadDeadZone(DX_INPUT_PAD1, System::Input::STICK_DEADZONE);
}

/*インスタンスを取得*/
CheckKey& CheckKey::GetIns() {
	static CheckKey instance;
	return instance;
}

/*前の入力状態として保存する関数*/
void CheckKey::LateInput() {
	//今の入力状態をPrevにコピー
	memcpy(Prevkeyboard, keyboard, sizeof(keyboard));
	prevMouseButton = mouseButton;
	Prevjoy = joy;
}

/*入力状態を得る関数*/
void CheckKey::Input() {
	GetHitKeyStateAll(keyboard);
	mouseButton = GetMouseInput();
	joy = GetJoypadInputState(DX_INPUT_PAD1);
	GetMousePoint(&MouseX, &MouseY);
	GetJoypadAnalogInput(&LstickX, &LstickY, DX_INPUT_PAD1);
	GetJoypadAnalogInputRight(&RstickX, &RstickY, DX_INPUT_PAD1);
	mouseWheelVol = GetMouseWheelRotVol();
}


/*キーボードの検知*/
bool CheckKey::isKeyHold(int code)const {		//入力がある
	return keyboard[code] != 0;

}

bool CheckKey::isKeyPress(int code)const {		//入力がある&前に入力がない
	return keyboard[code]!= 0&& Prevkeyboard[code] == 0;
}

bool CheckKey::isKeyRelease(int code)const {	//入力がない&前に入力がある
	return keyboard[code] == 0 && Prevkeyboard[code] != 0;
}

/*マウスのボタンの検知*/
bool CheckKey::IsmouseHold(int code)const {		
	if (code == System::Input::MOUSE_WHEEL_UP || code == System::Input::MOUSE_WHEEL_DOWN)return false;
	return(mouseButton & code) != 0;
}

bool CheckKey::IsmousePress(int code)const {
	if (code == System::Input::MOUSE_WHEEL_UP)return mouseWheelVol > 0;
	if (code == System::Input::MOUSE_WHEEL_DOWN)return mouseWheelVol < 0;
	return (mouseButton & code) != 0 && (prevMouseButton & code) == 0;
}

bool CheckKey::IsmouseRelease(int code)const {
	if (code == System::Input::MOUSE_WHEEL_UP || code == System::Input::MOUSE_WHEEL_DOWN)return false;
	return (mouseButton & code) == 0 && (prevMouseButton & code) != 0;
}

/*コントローラーの検知*/
bool CheckKey::IsjoyHold(int code)const{
	return(joy & code) != 0;
}

bool CheckKey::IsjoyPress(int code)const {
	return (joy & code) != 0 && (Prevjoy & code) == 0;
}
bool CheckKey::IsjoyRelease(int code)const {
	return (joy & code) == 0 && (Prevjoy & code) != 0;
}

/*押している間を検知する関数*/
bool CheckKey::isHold(InputType type, int code)const {
	switch (type) {		//引数の入力デバイスによってswitch
	case KEYBOARD:
		return isKeyHold(code);
	case MOUSE:
		return IsmouseHold(code);
	case JOY:
		return IsjoyHold(code); 
	default: 
		return false;
	}
}
/*押した瞬間を検知する関数*/
bool CheckKey::isPress(InputType type, int code)const {
	switch (type) {		
	case KEYBOARD:
		return isKeyPress(code);
	case MOUSE:
		return IsmousePress(code);
	case JOY:
		return IsjoyPress(code);
	default:
		return false;
	}
}
/*離した瞬間を検知する関数*/
bool CheckKey::isRelease(InputType type, int code)const {
	switch (type) {
	case KEYBOARD:
		return isKeyRelease(code);
	case MOUSE:
		return IsmouseRelease(code);
	case JOY:
		return IsjoyRelease(code);
	default:
		return false;
	}
}

/*カーソルの位置を取得する関数*/
void CheckKey::GetMousePosition(int& x, int& y) {
	x = MouseX;
	y = MouseY;
	return;
}

/*左スティックの入力を得る関数*/
void CheckKey::GetLeftStick(int& x, int& y) {
	x = LstickX;
	y = LstickY;
	return;
}

/*右スティックの入力を得る関数*/
void CheckKey::GetRightStick(int& x, int& y) {
	x = RstickX;
	y = RstickY;
	return;
}
