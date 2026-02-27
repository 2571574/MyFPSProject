#include "Parameter.h"
#include "DxLib.h"
#include "SceneManager.h"
#include "GameScene.h"
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    SetGraphMode(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_COLORBIT);	//ウィンドウサイズの設定
	ChangeWindowMode(TRUE); // ウィンドウモードに設定
	// DXライブラリの初期化
	if (DxLib_Init() == -1) {
		return -1; 
	}

	SetUseLighting(false);
	SetDrawScreen(DX_SCREEN_BACK); // 裏画面を描画対象にする
	SetUseZBuffer3D(TRUE);// 3D描画を有効にする
	SetWriteZBuffer3D(TRUE); // Zバッファへの書き込みを有効にする
    SetCameraNearFar(0.1f, 1000.0f);//オブジェクトを描画する距離を設定
	


	SceneManager ins;		//ゲームの処理のインスタンス
	// メインループ
	while (ProcessMessage() == 0 && CheckHitKey(KEY_INPUT_ESCAPE) == 0) {
		ClearDrawScreen();	//画面をクリア
		ins.Update();		//更新
		ins.Draw();		//描画
		ScreenFlip();
		if (ins.GetExitTag()) {
			break;
		}
	}
	// DXライブラリの終了処理
	DxLib_End();
	return 0;
}