#include "TextManager.h"
TextManager::TextManager() {
	weaponNames[WeaponID::AR] = "アサルトライフル";
	weaponNames[WeaponID::SR] = "スナイパーライフル";
	weaponNames[WeaponID::LR] = "ロケットランチャー";
	weaponNames[WeaponID::SMG] = "サブマシンガン";
	weaponNames[WeaponID::PIS] = "ハンドガン";
	actionNames[ActionID::MOVE_FORWARD] = "前進";
	actionNames[ActionID::MOVE_LEFT] = "左";
	actionNames[ActionID::MOVE_BACK] = "後退";
	actionNames[ActionID::MOVE_RIGHT] = "右";
	actionNames[ActionID::FIRE] = "射撃";
	actionNames[ActionID::ADS] = "狙う";
	actionNames[ActionID::RELOAD] = "リロード";
	actionNames[ActionID::RUN] = "ダッシュ";
	actionNames[ActionID::JUMP] = "ジャンプ";
	actionNames[ActionID::CROUCH] = "しゃがむ";
	actionNames[ActionID::WEAPON_NEXT] = "次の武器";
	actionNames[ActionID::WEAPON_PREV] = "前の武器";
	actionNames[ActionID::INTERACT] = "拾う";
	

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