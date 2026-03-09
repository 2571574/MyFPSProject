#pragma once
#include "DxLib.h"
#include "Parameter.h"

enum InputType {
	KEYBOARD,
	MOUSE,
	JOY
};


/// <summary>
/// 各デバイスの入力を全て管理するクラス
/// </summary>
class CheckKey
{
private:
	//現在の入力状態
	char keyboard[256];
	int mouseButton;
	int joy;

	//前の入力状態
	char Prevkeyboard[256];
	int prevMouseButton;
	int Prevjoy;

	//マウスやスティックの入力
	int MouseX, MouseY;
	int LstickX, LstickY;
	int RstickX, RstickY;


	//キーボード
	bool isKeyHold(int code) const;	
	bool isKeyPress(int code) const;
	bool isKeyRelease(int code) const;
	
	//マウス
	bool IsmouseHold(int code) const;
	bool IsmousePress(int code) const;
	bool IsmouseRelease(int code) const;

	//コントローラー
	bool IsjoyHold(int code)const;
	bool IsjoyPress(int code)const;	
	bool IsjoyRelease(int code)const;

	CheckKey();

public:
	
	static CheckKey& GetIns();

	/// <summary>
	/// 現在の入力を受け取る
	/// </summary>
	void Input();

	/// <summary>
	/// 現在の入力を前の入力状態として保存する
	/// </summary>
	void LateInput();

	/// <summary>
	/// 押している間を検知する
	/// </summary>
	/// <param name="type">入力デバイスの種類</param>
	/// <param name="code">検知するキー</param>
	/// <returns>検知するキーを押している間true</returns>
	bool isHold(InputType type, int code)const;

	/// <summary>
	/// 押した瞬間のみを検知する
	/// </summary>
	/// <param name="type">入力デバイスの種類</param>
	/// <param name="code">検知するキー</param>
	/// <returns>検知するキーを押した瞬間のみtrue</returns>
	bool isPress(InputType type, int code)const;

	/// <summary>
	///	離した瞬間のみを検知する
	/// </summary>
	/// <param name="type">入力デバイスの種類</param>
	/// <param name="code">検知するキー</param>
	/// <returns>検知するキーを離した瞬間のみtrue</returns>
	bool isRelease(InputType type, int code)const;

	/// <summary>
	/// マウスの座標を渡す
	/// </summary>
	/// <param name="x">x座標を入れる参照</param>
	/// <param name="y">y座標を入れる参照</param>
	void GetMousePosition(int& x, int& y);

	/// <summary>
	/// コントローラーのスティック入力を渡す　-1000～1000
	/// </summary>
	/// <param name="x">xの入力量を入れる参照</param>
	/// <param name="y">yの入力量を入れる参照</param>
	void GetLeftStick(int& x, int& y);

	/// <summary>
	/// コントローラーのスティック入力を渡す　-1000～1000
	/// </summary>
	/// <param name="x">xの入力量を入れる参照<param>
	/// <param name="y">yの入力量を入れる参照</param>
	void GetRightStick(int& x, int& y);
};

