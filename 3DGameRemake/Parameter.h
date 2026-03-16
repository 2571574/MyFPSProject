#pragma once

//描画
#define WINDOW_WIDTH 1920				//ウィンドウの幅
#define WINDOW_HEIGHT 1080				//ウィンドウの高さ
#define WINDOW_COLORBIT 32				//カラービット数
#define CENTER_X (WINDOW_WIDTH / 2)		//X軸画面中央
#define CENTER_Y (WINDOW_HEIGHT / 2)	//Y軸画面中央
#define CIRCLE_DIVNUM 16				//円の頂点の数
#define _BASE_FOV 70.0f * DX_PI_F / 180
#define MAX_FOV 110.0f * DX_PI_F / 180
#define RECOIL_SPEED 15.0f
#define RECOVERY_SPEED 3.0f


//入力
#define STICK_DEADZONE 0.01f			//スティックのデッドゾーン
#define _SENSITIVITY 0.1f				//マウス感度
#define _PAD_SENSITIVITY 0.002f			//スティック感度
#define EYE_HEIGHT 1.8f			//プレイヤーのカメラの高さ
#define CROUCH_EYE_HEIGHT 0.8f
#define CAM_ANGLESNAP_PITCH 89.9f		//カメラのy軸制限
#define CAM_ANGLESNAP_YAW 180.0f		//カメラのx軸制限
#define YAW_SNAP 360.0f					//x軸のスナップ用

