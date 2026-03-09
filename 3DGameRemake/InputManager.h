#pragma once
#include <map>
#include <vector>
#include "CheckKey.h"


struct KeyBind {
	InputType type;
	int KeyCode;
};

enum class ActionID {
	MOVE_FORWARD,
	MOVE_LEFT,
	MOVE_BACK,
	MOVE_RIGHT,
	FIRE,
	ADS,
	RELOAD,
	ITEMTHROW,
	RUN,
	JUMP,
	CROUCH,

	MENU_UP,
	MENU_LEFT,
	MENU_DOWN,
	MENU_RIGHT,
	MENU_SELECT,
	MENU_BACK,
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

	void SetBind(ActionID id, InputType type, int code);	//キーバインドをセット
};

