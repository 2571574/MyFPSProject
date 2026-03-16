#pragma once
#include<unordered_map>
#include<string>
#include"status.h"
#include "InputManager.h"
class TextManager
{
private:
	std::unordered_map<WeaponID, std::string>weaponNames;
	std::unordered_map<ActionID, std::string>actionNames;
	TextManager();

public:
	static TextManager& GetIns();

	const char* GetWeaponName(WeaponID id)const;
	const char* GetActionName(ActionID id)const;
};

