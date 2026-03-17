#pragma once
#include "BaseScene.h"
#include "SceneManager.h"
#include "InputManager.h"

#include<vector>
#include<string>

/// <summary>
/// タイトルシーンを管理するクラス
/// </summary>
class TitleScene:public BaseScene
{
private:

	//タイトルシーンの画面ステート
	enum class TitleState {
		TOP,
		MODE_SELECT,
		SETTINGS,
		KEY_CONFIG,
		CREDIT,
	};

	//トップメニュー
	enum PlayMenu {
		MENU_PLAY,
		MENU_SETTING,
		MENU_CREDIT,
		MENU_EXIT,
		MENU_MAX
	};

	//セッティングメニュー
	enum class SettingItem {
		BGM_VOLUME,
		SE_VOLUME,
		MOUSE_SENSITIVITY,
		PAD_SENSITIVITY,
		BASE_FOV,
		HEAD_BOB,
		RECOVERY,
		KEY_CONFIG,
		BACK,
		MAX
	};

	
	TitleState currentState;	//現在のステート
	int selectNum;				//現在選んでいる行
	int columnidx = 0;			//現在選んでいる列
	bool isWaitingKey = false;	//入力待ち　キーバインド用

	/// <summary>
	/// メニュー操作の入力処理
	/// </summary>
	void Control();

	/// <summary>
	/// 次のステートに移動する
	/// </summary>
	/// <param name="next">移動先のステート</param>
	void ChangeState(TitleState next);

	/// <summary>
	/// キーバインドで重複したバインドを削除する
	/// </summary>
	/// <param name="type">入力デバイスのタイプ</param>
	/// <param name="code">入力したキー</param>
	void RemoveDuplicateBind(InputType type,int code);

	/// <summary>
	/// 設定項目の値を変更する
	/// </summary>
	/// <param name="item">項目</param>
	/// <param name="direction">増減方向</param>
	void ModifySetting(SettingItem item, int direction);

	/// <summary>
	/// 設定の描画処理　分割用
	/// </summary>
	void DrawSettings();

	/// <summary>
	/// キーバインドの描画処理　分割用
	/// </summary>
	void DrawKeyConfig();
public:

	TitleScene(SceneManager* manager);
	~TitleScene()override;

	void Init()override;
	void Update()override;
	void Draw()override;

};