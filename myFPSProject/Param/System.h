#pragma once
#include "Param/Global.h"
#include "Param/Chara.h"

namespace System {

    namespace Window {
        constexpr int WINDOW_WIDTH = 1920;      //ウィンドウの横幅
        constexpr int WINDOW_HEIGHT = 1080;     //ウィンドウの縦幅
        constexpr int WINDOW_COLORBIT = 32;     //ウィンドウの色深度
        constexpr int CENTER_X = WINDOW_WIDTH / 2;  //画面中央のX座標
        constexpr int CENTER_Y = WINDOW_HEIGHT / 2; //画面中央のY座標
        constexpr int CIRCLE_DIVNUM = 16;           //図形オブジェクトの分割数
    }

    namespace Time {
        constexpr float MAX_DELTA_TIME = 0.1f;      //1フレームの経過時間の最大
    }

    namespace Camera {
        constexpr float DEFAULT_CAM_X = 180.0f;         //スポーン時のデフォルトyaw
        constexpr float CAM_ANGLESNAP_PITCH = 89.9f;    //カメラの垂直の最大角度
        constexpr float CAM_ANGLESNAP_YAW = 180.0f;     //カメラの水平角度の限界角度
        constexpr float YAW_SNAP = 360.0f;              //水平角度を360でスナップする数値
        constexpr float RECOIL_SPEED = 15.0f;           //カメラの跳ね上がり速度
        constexpr float RECOVERY_SPEED = 3.0f;          //反動のリカバリー速度
        constexpr float CAMERA_NEAR_CLIP = 0.05f;       //描画の最短距離
        constexpr float CAMERA_FAR_CLIP = 1000.0f;      //描画の最大距離
    }

    namespace Pathfinding {
        constexpr float GRID_SIZE = 2.0f;                           //ノードを配置するグリッドのサイズ
        constexpr float CHARA_HEIGHT = Chara::Base::CHARA_HEIGHT;   //ノードの安全チェックに使うキャラの高さ
        constexpr float CHARA_RADIUS = Chara::Base::CHARA_RADIUS;   //ノードの安全チェックに使うキャラの半径
        constexpr float MAP_START_Y = 30.0f;                        //ノード配置のレイの発射高さ
        constexpr float MAP_END_Y = Global::World::Y_MIN;           //ノード配置のレイの終点高さ
        constexpr float MAX_SLOPE_NORMAL = 0.7f;                    //歩行可能とみなす傾斜の最大法線Yベクトル
        constexpr float MAP_MIN_X = -30.0f;                         //マップのX始点
        constexpr float MAP_MAX_X = 30.0f;                          //マップのX終点
        constexpr float MAP_MIN_Z = -30.0f;                         //マップのZ始点
        constexpr float MAP_MAX_Z = 30.0f;                          //マップのZ終点
        constexpr float RAY_Y_OFFSET = 1.0f;                        //床の安全チェックに使うレイのYオフセット
        constexpr float GROUND_CHECK_DIST = 0.4f;                   //ノード間の歩行チェックするときの間隔

        constexpr int CIRCLE_CHECK_SEGMENTS = 8;                    //ノードの着地チェックする時の円周上のレイ本数
        constexpr float CAPSULE_BOTTOM_MARGIN = 0.1f;               //カプセル判定時の底面の余裕
        constexpr float NODE_PLACEMENT_OFFSET_Y = 0.1f;             //ノードの実際の配置Yオフセット
        constexpr float NEXT_RAY_OFFSET_Y = -0.5f;                  //重なった床の下の床を探す時のレイオフセット
        constexpr float NODE_CONNECT_MAX_DIST_RATE = 1.8f;          //GRID_SIZEに対するノードを繋げる時の最大距離倍率
        constexpr float WALKABLE_HEIGHT_TOLERANCE = 0.2f;           //歩行可能な段差の許容高さ
        constexpr float GROUND_CHECK_RAY_OFFSET_Y_TOP = 0.5f;       //経路チェック時のレイ始点高さ
        constexpr float GROUND_CHECK_RAY_OFFSET_Y_BOTTOM = -0.5f;   //経路チェック時のレイ終点高さ
        constexpr float MAX_DISTANCE_SQUARED = 99999.0f;            //最短ノード探索の初期最短距離

        constexpr int MIN_DIV_COUNT = 1;                            //経路上の地面チェックの分割数
    }

    namespace Sound {
        constexpr int MAX_DUPLICATE = 8;                            //同じSEの再生数の最大
        constexpr int MAX_VOLUME_SCALE = 255;                       //音量の最大値
    }

    //各設定のデフォルト値
    namespace Config {
        constexpr float DEFAULT_BGM_VOLUME = 0.5f;
        constexpr float DEFAULT_SE_VOLUME = 0.5f;
        constexpr float DEFAULT_MOUSE_SENSITIVITY = 0.050f;
        constexpr float DEFAULT_PAD_SENSITIVITY = 0.005f;
        constexpr float DEFAULT_BASE_FOV = 70.0f;
        constexpr bool DEFAULT_HEADBOB = true;
        constexpr bool DEFAULT_RECOVERY = false;
    }

    namespace Collision {
        constexpr float MIN_HEAD_RAD = 0.15f;               //ヘッド判定の最小半径
        constexpr float HITSCAN_RAY_THICKNESS = 0.05f;      //ヒットスキャン武器のレイの幅
        constexpr float MIN_DIST_SQUARED = 0.01f;           //ゼロ除算防止のための最小距離
        constexpr float EXPLODE_UPWARD_BIAS = 0.5f;         //爆発ノックバックの上方向バイアス
        constexpr float MIN_NECK_HEIGHT_BODYRAD_MULT = 2.0f;//首の高さが確保できない時の高さ倍率
        constexpr float PUSH_SPLIT_RATIO_EQUAL_MASS = 0.5f; //同質量のキャラの押出量の分配
        constexpr int MIN_DAMAGE_ON_FALLOFF = 1;            //ダメージ減衰の最低保証ダメージ
        constexpr float DEFAULT_FALLBACK_DIR_Y = 1.0f;      //爆発中心とキャラが完全に重なった場合のノックバック方向(Y)
    }

    namespace Input {
        constexpr float STICK_DEADZONE = 0.01f;        //スティックのデッドゾーン
        constexpr int MOUSE_WHEEL_UP = 1301;           //マウスホイール上のキーコード
        constexpr int MOUSE_WHEEL_DOWN = 1302;         //マウスホイール下のキーコード
        constexpr int MAX_KEYCODES = 256;              //キーボードのキーコード最大数
    }

}