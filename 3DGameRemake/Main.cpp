#include "Parameter.h"
#include "DxLib.h"
#include "SceneManager.h"
#include "CheckKey.h"
#include "ConfigManager.h"
#include "SoundManager.h"
#include "Time.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    SetGraphMode(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_COLORBIT);	//ウィンドウサイズの設定
	SetMainWindowText("Killing Arena");
	// DXライブラリの初期化
	if (DxLib_Init() == -1) {
		return -1; 
	}

	SetCreate3DSoundFlag(TRUE);
	SetUseLighting(false);		 //ライティングの無効化
	SetDrawScreen(DX_SCREEN_BACK); // 裏画面を描画対象にする

	SetUseZBuffer3D(TRUE);// 3D描画を有効にする
	SetWriteZBuffer3D(TRUE); // Zバッファへの書き込みを有効にする

    SetCameraNearFar(0.01f, 1000.0f);//オブジェクトを描画する距離を設定
	
	
	ConfigManager::GetIns().Load();	//Configの読み込み

	SceneManager ins;		//ゲームの処理のインスタンス

	// メインループ
	while (ProcessMessage() == 0) {
		Time::GetIns().Update();
		CheckKey::GetIns().Input();	//入力を取得
		ClearDrawScreen();	//画面をクリア
		SoundManager::GetIns().Update();
		ins.Update();		//更新
		ins.Draw();		//描画
		ScreenFlip();
		if (ins.GetExitTag()) {
			break;
		}
		CheckKey::GetIns().LateInput();	//入力を保持
	}
	// DXライブラリの終了処理
	DxLib_End();
	return 0;
}