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