#include "DxLib.h"
#include "Param/Global.h"
#include "Param/System.h"
#include "Status.h"
#include "CheckKey.h"
#include "SceneManager.h"
#include "ConfigManager.h"
#include "SoundManager.h"
#include "Time.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	SetGraphMode(System::Window::WINDOW_WIDTH, System::Window::WINDOW_HEIGHT, System::Window::WINDOW_COLORBIT);
	SetMainWindowText("Killing Arena");
	SetEnableXAudioFlag(TRUE);	//サウンドにXAudioを使用
	SetWaitVSyncFlag(FALSE);	//垂直同期はオフ

	// DXライブラリの初期化
	if (DxLib_Init() == -1) {
		return -1;
	}

	SetMouseDispFlag(FALSE);	//マウスカーソルの無効化
	SetUseLighting(false);		 //ライティングの無効化
	SetDrawScreen(DX_SCREEN_BACK); // 裏画面を描画対象にする

	SetUseZBuffer3D(TRUE);// 3D描画を有効にする
	SetWriteZBuffer3D(TRUE); // Zバッファへの書き込みを有効にする

	SetCameraNearFar(System::Camera::CAMERA_NEAR_CLIP, System::Camera::CAMERA_FAR_CLIP);//オブジェクトを描画する距離を設定

	ConfigManager::GetIns().Load();	//Configの読み込み

	LoadAllStatusFromCSV();	//ステータスを読み込む
	SceneManager ins;		//ゲームの処理のインスタンス

	// メインループ
	while (ProcessMessage() == 0) {
		Time::GetIns().Update();
		CheckKey::GetIns().Input();	//入力を取得
		ClearDrawScreen();
		SoundManager::GetIns().Update();
		ins.Update();
		ins.Draw();	
		ScreenFlip();
		if (ins.GetExitTag()) {
			break;
		}
		CheckKey::GetIns().LateInput();	//入力を保持
	}
	DxLib_End();
	return 0;
}