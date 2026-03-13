#include "TextManager.h"
TextManager::TextManager() {
	weaponNames[WeaponID::AR] = "Assault Rifle";
	weaponNames[WeaponID::SR] = "Sniper Rifle";
	weaponNames[WeaponID::LR] = "Rocket Launcher";
	weaponNames[WeaponID::SMG] = "Submashine Gun";
	weaponNames[WeaponID::PIS] = "Handgun";
	
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