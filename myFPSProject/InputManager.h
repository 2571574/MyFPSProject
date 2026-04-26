#pragma once
#include "CheckKey.h"

#include <map>
#include <vector>

//キーバインド保存用の構造体
struct KeyBind {
	InputType type;
	int KeyCode;
};

//アクションのID
enum class ActionID {
	MOVE_FORWARD,
	MOVE_LEFT,
	MOVE_BACK,
	MOVE_RIGHT,
	FIRE,
	ADS,
	RELOAD,
	RUN,
	JUMP,
	CROUCH,
	WEAPON_NEXT,
	WEAPON_PREV,
	INTERACT,
	MENU_UP,
	MENU_LEFT,
	MENU_DOWN,
	MENU_RIGHT,
	MENU_SELECT,
	MENU_BACK,
	PAUSE
};

/// <summary>
///	キー入力をアクションに変換するクラス
/// </summary>
class InputManager
{
private:
	InputManager();

	std::map<ActionID, std::vector<KeyBind>> bindings;	//キーバインド

public:
	static InputManager& GetIns();

	/// <summary>
	/// 全てのアクションをデフォルトのキーバインドにする。
	/// </summary>
	void SetDefaultBinding();

	/// <summary>
	/// 押している間を検知する
	/// </summary>
	/// <param name="id">検知したいアクション</param>
	/// <returns>押している間true</returns>
	bool IsActionHold(ActionID id)const;

	/// <summary>
	/// 押した瞬間を検知する
	/// </summary>
	/// <param name="id">検知したいアクション</param>
	/// <returns>押した瞬間のみtrue</returns>
	bool IsActionTrigger(ActionID id)const;



	void SetBind(ActionID id, InputType type, int code);
	void SetBind(const std::map < ActionID, std::vector<KeyBind>>& newBindings) { bindings = newBindings; }

	std::map<ActionID, std::vector<KeyBind>>& GetBinds() { return bindings; }
};

