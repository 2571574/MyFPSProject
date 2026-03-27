#include "TextManager.h"
TextManager::TextManager() {
	weaponNames = {
		{WeaponID::AR,"アサルトライフル"},
		{WeaponID::SR,"スナイパーライフル"},
		{WeaponID::LR, "ロケットランチャー"},
		{WeaponID::SMG,"サブマシンガン"},
		{WeaponID::PIS, "ハンドガン"},
	};
	
	actionNames = {
		{ActionID::MOVE_FORWARD, "前進"},
		{ActionID::MOVE_LEFT,"左"},
		{ActionID::MOVE_BACK,"後退"},
		{ActionID::MOVE_RIGHT,"右"},
		{ActionID::FIRE,"射撃"},
		{ActionID::ADS,"狙う"},
		{ActionID::RELOAD, "リロード"},
		{ActionID::RUN,"ダッシュ"},
		{ActionID::JUMP,"ジャンプ"},
		{ActionID::CROUCH,"しゃがむ"},
		{ActionID::WEAPON_NEXT,"次の武器"},
		{ActionID::WEAPON_PREV, "前の武器"},
		{ActionID::INTERACT,"拾う"}
	};
	deathCause = {
		{WeaponID::ENEMY_KNIFE, "死因:格闘"},
		{WeaponID::ENEMY_AR,"死因:ライフル"},
		{WeaponID::ENEMY_SR,"死因:スナイパー"},
		{WeaponID::ENEMY_EXPLOSION,"死因:爆発"},
		{WeaponID::LR,"死因:自爆"}
	};

	menuDescription = {
		{0,"ゲームを開始します。"},
		{1,"設定を変更します。"},
		{2,"クレジットを表示します。"},
		{3,"ゲームを終了します。"},
		{4,"BGMの音量を変更します。"},
		{5,"SEの音量を変更します。"},
		{6,"マウスの感度を変更します。"},
		{7,"コントローラーの感度を変更します。"},
		{8,"視野角を変更します。"},
		{9,"移動時の画面の揺れを変更します。"},
		{10,"銃の反動がある程度戻るようになります。"},
		{11,"キーバインドを編集します。"},
		{12,""},
		{13,"チュートリアル"},
		{14," イージー\n\n 最初から武器を持ち、弾が無限になります。"},
		{15," ノーマル\n\n 弾が有限になり、武器を拾いながら戦います。"},
		{16," ハード\n\n 弾は有限、持てる武器が２つまでになります。"},
	};
}

TextManager& TextManager::GetIns() {
	static TextManager ins;
	return ins;
}

const char* TextManager::GetWeaponName(WeaponID id)const {
	auto it = weaponNames.find(id);
	if (it != weaponNames.end()) {
		return it->second.c_str();
	}
	return "Unknown";
}

const char* TextManager::GetActionName(ActionID id)const {
	auto it = actionNames.find(id);
	if (it != actionNames.end()) {
		return it->second.c_str();
	}
	return "Unknown";
}

const char* TextManager::GetCauseName(WeaponID id)const {
	auto it = deathCause.find(id);
	if (it != deathCause.end()) {
		return it->second.c_str();
	}
	return "Unknown";
}

const char* TextManager::GetKeyName(InputType type, int code) const {
	if (type == InputType::MOUSE) {
		if (code == MOUSE_INPUT_LEFT) return "左クリック";
		if (code == MOUSE_INPUT_RIGHT) return "右クリック";
		if (code == MOUSE_INPUT_MIDDLE) return "中クリック";
		if (code == System::Input::MOUSE_WHEEL_UP) return "ホイール上";
		if (code == System::Input::MOUSE_WHEEL_DOWN) return "ホイール下";
		if (code == MOUSE_INPUT_4) return "サイド 1";
		if (code == MOUSE_INPUT_5) return "サイド 2";
		return "マウス";
	}
	else if (type == InputType::JOY) {
		if (code == PAD_INPUT_DOWN) return "下ボタン";
		if (code == PAD_INPUT_LEFT) return "左ボタン";
		if (code == PAD_INPUT_RIGHT) return "右ボタン";
		if (code == PAD_INPUT_UP) return "上ボタン";
		if (code == PAD_INPUT_1) return "ボタン1";
		if (code == PAD_INPUT_2) return "ボタン2";
		if (code == PAD_INPUT_3) return "ボタン3";
		if (code == PAD_INPUT_4) return "ボタン4";
		if (code == PAD_INPUT_5) return "ボタン5";
		if (code == PAD_INPUT_6) return "ボタン6";
		if (code == PAD_INPUT_7) return "ボタン7";
		if (code == PAD_INPUT_8) return "ボタン8";
		if (code == PAD_INPUT_9) return "ボタン9";
		if (code == PAD_INPUT_10) return "ボタン10";
		return "PADボタン";
	}
	else if (type == InputType::KEYBOARD) {
		switch (code) {
		case KEY_INPUT_A: return "A";
		case KEY_INPUT_B: return "B";
		case KEY_INPUT_C: return "C";
		case KEY_INPUT_D: return "D";
		case KEY_INPUT_E: return "E";
		case KEY_INPUT_F: return "F";
		case KEY_INPUT_G: return "G";
		case KEY_INPUT_H: return "H";
		case KEY_INPUT_I: return "I";
		case KEY_INPUT_J: return "J";
		case KEY_INPUT_K: return "K";
		case KEY_INPUT_L: return "L";
		case KEY_INPUT_M: return "M";
		case KEY_INPUT_N: return "N";
		case KEY_INPUT_O: return "O";
		case KEY_INPUT_P: return "P";
		case KEY_INPUT_Q: return "Q";
		case KEY_INPUT_R: return "R";
		case KEY_INPUT_S: return "S";
		case KEY_INPUT_T: return "T";
		case KEY_INPUT_U: return "U";
		case KEY_INPUT_V: return "V";
		case KEY_INPUT_W: return "W";
		case KEY_INPUT_X: return "X";
		case KEY_INPUT_Y: return "Y";
		case KEY_INPUT_Z: return "Z";
		case KEY_INPUT_0: return "0";
		case KEY_INPUT_1: return "1";
		case KEY_INPUT_2: return "2";
		case KEY_INPUT_3: return "3";
		case KEY_INPUT_4: return "4";
		case KEY_INPUT_5: return "5";
		case KEY_INPUT_6: return "6";
		case KEY_INPUT_7: return "7";
		case KEY_INPUT_8: return "8";
		case KEY_INPUT_9: return "9";
		case KEY_INPUT_SPACE: return "SPACE";
		case KEY_INPUT_LSHIFT: return "L-SHIFT";
		case KEY_INPUT_RSHIFT: return "R-SHIFT";
		case KEY_INPUT_LCONTROL: return "L-CTRL";
		case KEY_INPUT_RCONTROL: return "R-CTRL";
		case KEY_INPUT_ESCAPE: return "ESC";
		case KEY_INPUT_RETURN: return "ENTER";
		case KEY_INPUT_TAB: return "TAB";
		case KEY_INPUT_UP: return "UP";
		case KEY_INPUT_DOWN: return "DOWN";
		case KEY_INPUT_LEFT: return "LEFT";
		case KEY_INPUT_RIGHT: return "RIGHT";
		default: return "KEY";
		}
	}
	return "UNKNOWN";
}


std::string TextManager::GetActionKeyString(ActionID id) const {
	auto& binds = InputManager::GetIns().GetBinds();
	std::string kbStr = "NONE";
	std::string padStr = "NONE";

	if (binds.count(id)) {
		for (const auto& b : binds.at(id)) {
			if (b.type == InputType::KEYBOARD || b.type == InputType::MOUSE) {
				kbStr = GetKeyName(b.type, b.KeyCode);
			}
			if (b.type == InputType::JOY) {
				padStr = GetKeyName(b.type, b.KeyCode);
			}
		}
	}
	if (kbStr == "NONE" && padStr == "NONE") return "未設定";
	if (kbStr == "NONE") return padStr;
	if (padStr == "NONE") return kbStr;

	return kbStr + " / " + padStr;
}


const char* TextManager::GetMenuDescription(int id)const {
	auto it = menuDescription.find(id);
	if (it != menuDescription.end()) {
		return it->second.c_str();
	}
	return "";
}