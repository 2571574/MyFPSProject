#pragma once
#include "DxLib.h"
#include "Parameter.h"

//入力デバイス
enum InputType {
	KEYBOARD,
	MOUSE,
	JOY
};

/*入力を受け取るクラス*/
class CheckKey
{
private:
	//ボタン入力を保持
	char keyboard[256];
	int mouseButton;
	int joy;

	//前のフレームのボタン入力を保持
	char Prevkeyboard[256];
	int prevMouseButton;
	int Prevjoy;

	//センサーやスティックの入力を保持
	int MouseX, MouseY;
	int LstickX, LstickY;
	int RstickX, RstickY;


	//キーボード
	bool isKeyHold(int code) const;		//押しているかを検知
	bool isKeyPress(int code) const;		//押した瞬間を検知
	bool isKeyRelease(int code) const;	//離した瞬間を検知
	
	//マウス
	bool IsmouseHold(int code) const;	//押しているかを検知
	bool IsmousePress(int code) const;	//押した瞬間を検知
	bool IsmouseRelease(int code) const;	//離した瞬間を検知

	//コントローラー
	bool IsjoyHold(int code)const;		//押しているかを検知
	bool IsjoyPress(int code)const;		//押した瞬間を検知
	bool IsjoyRelease(int code)const;	//離した瞬間を検知

	CheckKey();		//コンストラクタ

public:
	static CheckKey& GetIns();	//インスタンスを取得
	void Input();		//入力を取得
	void LateInput();	//前の入力状態として保存

	//入力状態を検知する関数
	bool isHold(InputType type, int code)const;		//押している間
	bool isPress(InputType type, int code)const;	//押した瞬間
	bool isRelease(InputType type, int code)const;	//離した瞬間

	//カーソルの位置を取得する関数
	void GetMousePosition(int& x, int& y);

	//スティックの入力を取得する関数
	void GetLeftStick(int& x, int& y);
	void GetRightStick(int& x, int& y);
};

