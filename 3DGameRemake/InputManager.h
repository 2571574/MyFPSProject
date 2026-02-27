#pragma once
#include <map>
#include <vector>
#include "CheckKey.h"

//キーバインドの構造体
struct KeyBind {
	InputType type;
	int KeyCode;
};

//操作を指定するID
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


/*キーとアクションを紐づけるクラス*/
class InputManager
{
private:
	InputManager();	//コンストラクタ

	std::map<ActionID, std::vector<KeyBind>> bindings;	//キーバインド

public:
	static InputManager& GetIns();	//インスタンスを取得

	void SetDefaultBinding();		//キーバインドをデフォルトにする

	bool IsActionHold(ActionID id)const;	//ホールドでアクション
	bool IsActionTrigger(ActionID id)const;	//トリガーでアクション
	void SetBind(ActionID id, InputType type, int code);	//キーバインドをセット
};

