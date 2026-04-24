#pragma once
#include<unordered_map>
#include<string>
#include"status.h"
#include "InputManager.h"

/// <summary>
/// ゲーム内オブジェクト名のテキストを管理するクラス
/// </summary>
class TextManager
{
private:
	std::unordered_map<WeaponID, std::string>weaponNames;		//武器の名前
	std::unordered_map<ActionID, std::string>actionNames;		//行動名
	std::unordered_map<WeaponID, std::string>deathCause;		//死因名
	std::unordered_map<int, std::string> menuDescription;		//ダイアログの中身
	TextManager();

public:
	static TextManager& GetIns();

	const char* GetWeaponName(WeaponID id)const;
	const char* GetActionName(ActionID id)const;
	const char* GetCauseName(WeaponID id)const;
	const char* GetKeyName(InputType type, int code)const;
	std::string GetActionKeyString(ActionID id)const;

	const char* GetMenuDescription(int id)const;
};

