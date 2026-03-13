#pragma once
#include<unordered_map>
#include<string>
#include"status.h"
class TextManager
{
private:
	std::unordered_map<WeaponID, std::string>weaponNames;
	TextManager();

public:
	static TextManager& GetIns();

	const char* GetWeaponName(WeaponID id)const;
};

