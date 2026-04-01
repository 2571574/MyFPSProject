#pragma once
#include "Param/Global.h"

//キャラクター関係の定数
namespace Chara {

    namespace Base {
        constexpr float CHARA_HEIGHT = 2.0f;            //キャラのデフォルト身長
        constexpr float CHARA_RADIUS = 0.5f;            //キャラのデフォルト幅

        constexpr float GRAVITY = -0.008f;              //重力
        constexpr float GROUND_KB_FRICTION = 0.8f;      //ノックバック時の地上摩擦
        constexpr float AIR_KB_FRICTION = 0.98f;        //ノックバック時の空中摩擦
        constexpr float GROUND_NORMAL_MIN = 0.3f;       //床とみなす法線ベクトルの角度下限
        constexpr float CEILING_NORMAL_MAX = -0.1f;     //天井とみなす法線ベクトルの角度上限

        constexpr int RAY_COUNT = 5;                    //キャラの着地判定のレイの本数
        constexpr float STEP_RAY_START = 0.1f;          //着地判定始点高さ
        constexpr float STEP_RAY_END = -0.2f;           //着地判定終点高さ
        constexpr float CAP_SIDE_OFFSET = 0.8f;         //着地判定のレイの間隔
        constexpr float CAP_BOTTOM_OFFSET = 0.3f;       //壁判定のためのカプセル下端オフセット

        constexpr int SHADOW_ALPHA = 100;               //影のα値
        constexpr float SHADOW_OFFSET_Y_HIGH = 0.02f;   //影描画の浮かせ幅（高）
        constexpr float SHADOW_OFFSET_Y_LOW = 0.01f;    //影描画の浮かせ幅（低）
        constexpr float SHADOW_WIDTH_DIVISOR = 1.5f;    //影の横幅

        constexpr float MOVEMENT_MIN = 0.01f;           //移動とみなす最小速度
    }

    //プレイヤーの定数
    namespace Player {
        constexpr float CROUCH_ACCEL_RATE = 0.08f;      //しゃがみの加速倍率
        constexpr float CROUCH_FRICTION_ADD = 0.08f;    //しゃがみの摩擦係数補正
        constexpr float SLIDE_MIN_SPEED = 0.18f;        //スライディングに必要な速度
        constexpr float SLIDE_BOOST = 2.5f;             //スライディングの加算速度
        constexpr float SLIDE_COOLDOWN = 5.0f;          //スライディングのクールタイム
        constexpr float RUN_ACCEL_ADD = 0.02f;          //ダッシュの速度加算
        constexpr float RUN_FRICTION_SUB = 0.05f;       //ダッシュの摩擦係数補正
        constexpr float JUMP_POWER = 0.25f;             //ジャンプ力
        constexpr float AIR_ACCEL_RATE = 0.1f;          //空中の摩擦係数補正
        constexpr float FOV_LERP_RATE = 0.1f;           //視野角の収束率
        constexpr float FOV_SPEED_BASE = 0.75f;         //速度による視野角変化の基準値
        constexpr float MAX_FOV = 110.0f * Global::Math::DEG_TO_RAD;    //最大視野角
        constexpr float BOBBING_SPEED_MULT = 1.3f;      //画面揺れの速度
        constexpr float BOBBING_AMPLITUDE = 0.05f;      //画面揺れの強さ
        constexpr float BOBBING_DECAY = 0.7f;           //停止時の画面揺れの収束率
        constexpr float BOBBING_CROUCH_MIN_SPEED = 0.06f;// しゃがみ時の揺れが有効になる時の最低速度

        constexpr float DROP_ITEM_Y_OFFSET = 0.4f;              //ドロップアイテムのYオフセット
        constexpr float STEP_LENGTH = 4.0f;                     //1歩の長さ
        constexpr float STICK_INPUT_SCALE = 1000.0f;            //スティック入力を正規化するためのスケール
        constexpr float INPUT_VECTOR = 1.0f;                    //移動ベクトル正規化の閾値
        constexpr float YAW_HALF_TURN_DEG = 180.0f;             //視点半周
        constexpr float YAW_FULL_TURN_DEG = 360.0f;             //視点１周
        constexpr float RECOIL_HIPFIRE_MULTIPLIER = 0.5f;       //腰撃ち中のリコイルの大きさ減衰量
        constexpr float ADS_SWAY_SCALE = 0.1f;                  //ADS中の武器揺れ補正
        constexpr float BOBBING_WEAPON_AMPLITUDE = 0.015f;      //移動の武器揺れの強さ
        constexpr float BOBBING_WEAPON_FREQUENCY_MULT = 0.5f;   //移動の武器揺れ速度
        constexpr int MAX_WEAPON_SLOT_EASY_NORMAL = 10;         //イージーとノーマルの時の武器スロット数
        constexpr int MAX_WEAPON_SLOT_HARD = 2;                 //ハードの時の武器スロット数
        constexpr float SWAY_MULTIPLIER = 0.02f;                //視点の武器揺れ強さ
        constexpr float MAX_SWAY = 0.5f;                        //視点の武器揺れの最大
        constexpr float SWAY_LERP_SPEED = 0.2f;                 //視点の武器揺れの収束率
    }

    namespace EnemyCommon {
        constexpr int PATH_UPDATE_RANDOM = 50;                  //更新までのミリ秒数の最大
        constexpr float PATH_NODE_REACHED_DIST = 1.0f;          //つながるノードの最大距離

        constexpr float PATH_SAFETY_RAY_HEIGHT = 1.0f;          //ノードの床チェックのレイの高さ
        constexpr float PATH_SAFETY_NORMAL_MIN = 0.6f;          //床チェックの法線角度の下限
        constexpr float PATH_SAFETY_HEIGHT_DIFF_MAX = 0.8f;     //経路の段差の許容高さ
        constexpr float PATH_SAFETY_TOTAL_HEIGHT_DIFF = 1.0f;   //ターゲットとの高低差の許容範囲
        constexpr float FALL_DEATH_Y = Global::World::Y_MIN;    //落下死と判定するY座標
        constexpr float STEP_LENGTH = 2.5f;                     //１歩の長さ

        constexpr float PATH_UPDATE_BASE_INTERVAL = 0.5f;       //経路再計算の基本間隔
        constexpr float PATH_SAFETY_SAMPLE_STEP = 1.0f;         //経路の安全性チェックの距離刻み
        constexpr float FOOTSTEP_SOUND_RADIUS = 30.0f;          //足音の聞こえる半径

        constexpr float LEAN_FACTOR = 1.5f;                     //速度に応じた傾きの具合
        constexpr float SPAWN_PROCESS_TIME = 1.5f;              //出現直後の行動しない無敵時間

        constexpr float HIT_FLASH_DURATION = 0.1f;          //ヒットエフェクトの時間
    }

    namespace EnemyManager {
        constexpr float BURST_SPAWN_THRESHOLD = 0.5f;           //この秒数以下なら一度に複数スポーンさせる
        constexpr int NORMAL_SPAWNCOUNT = 3;                    //一度にスポーンさせる基本数
        constexpr float SPAWNDIST_PLAYER = 10.0f;               //プレイヤーから離す基本距離
        constexpr float SPAWNDIST_ENEMY = 2.0f;                 //敵から離す基本距離（重なり防止）
        constexpr float SPAWN_RAY_START = 5.0f;                 //スポーン位置決定の接地確認レイ始点高さ
        constexpr float SPAWN_RAY_END = Global::World::Y_MIN;   //スポーン位置決定の接地確認レイ終点高さ
        constexpr float SPAWN_SOUND_RADIUS = 30.0f;             //スポーン音が聞こえる半径
    }

    namespace Melee {
        constexpr float ATTACK_WINDUP_TIME = 0.3f;                          //攻撃前の振り上げ
        constexpr float ATTACK_RECOVERY_TIME = 0.5f;                        //攻撃後の硬直
        constexpr Global::ColorRGB COLOR_NORMAL = Global::Palette::YELLOW;  //色
        constexpr float RECOVERY_PHASE1_RATIO = 0.3f;                       //攻撃後のアニメーションフェーズ1の割合
        constexpr float RECOVERY_PHASE2_RATIO = 0.7f;                       //攻撃後のアニメーションフェーズ2の割合
        constexpr float SWING_90_DEG = Global::Math::MATH_PI_HALF;          //武器を振る角度
        constexpr float SWING_QUARTER_TURN = Global::Math::MATH_PI_QUARTER; //武器を戻す時の角度

        constexpr float MOVE_STOP_RANGE_RATIO = 0.9f;                       //攻撃射程の何割で止まるかの比率
        constexpr float MELEE_ATTACK_SOUND_RADIUS = 20.0f;                  //攻撃音が聞こえる半径
    }

    namespace Rifle {
        constexpr float STOP_DISTANCE_RATIO = 0.6f;                         //攻撃射程の何割で止まるかの比率
        constexpr Global::ColorRGB COLOR_NORMAL = Global::Palette::BLUE;    //色
    }

    namespace Sniper {
        constexpr float ATTACK_DISTANCE_RATIO = 0.8f;                       //射程の何割から攻撃を開始する比率
        constexpr float ESCAPE_DISTANCE_RATIO = 0.4f;                       //射程の何割からプレイヤーから離れる比率
        constexpr float ESCAPE_CHECK_DIST = 2.0f;                           //逃走経路を確認する時の距離
        constexpr float TARGET_TIME = 4.5f;                                 //照準を合わせてから撃つまでの時間
        constexpr float INDICATOR_PHASE1_PROGRESS_RATIO = 0.6f;             //チャージの第１段階の割合
        constexpr float INDICATOR_PHASE2_PROGRESS_RATIO = 0.4f;             //チャージの第２段階の割合
        constexpr int INDICATOR_OUTER_ALPHA_PHASE1_MAX = 150;               //フェーズ1のレーザーの透明度
        constexpr float INDICATOR_OUTER_RADIUS_PHASE1 = 0.03f;              //フェーズ1のレーザーの太さ
        constexpr float INDICATOR_INNER_RADIUS_PHASE1 = 0.01f;              //フェーズ1のレーザーの芯の太さ
        constexpr Global::ColorRGB INDICATOR_OUTER_COLOR_PHASE1 = Global::Palette::CYAN;        //フェーズ1のレーザーの色
        constexpr int INDICATOR_OUTER_ALPHA_PHASE2 = 200;                   //フェーズ2の透明度
        constexpr float INDICATOR_OUTER_RADIUS_PHASE2_BASE = 0.05f;         //フェーズ2のレーザーの基準の太さ
        constexpr float INDICATOR_INNER_RADIUS_PHASE2 = 0.02f;              //フェーズ2のレーザーの芯の太さ
        constexpr float INDICATOR_OUTER_RADIUS_WAVE_AMPL = 0.02f;           //レーザーの波打ちの振幅
        constexpr float INDICATOR_WAVE_FREQ_MULT = 8.0f;                    //レーザーの波打つ速度
        constexpr Global::ColorRGB INDICATOR_OUTER_COLOR_PHASE2_BASE = Global::Palette::ORANGE_BRIGHT;          //フェーズ2のレーザーの色
        constexpr int INDICATOR_CAPSULE_SEGMENTS = 8;                       //レーザーの描画の分割数

        constexpr float CHARGE_SOUND_RADIUS = 40.0f;                        //チャージ音が聞こえる半径
    }

    namespace Rolling {
        constexpr float EXPLODE_TIME = 2.0f;                                //爆発までの時間
        constexpr float TRIGGER_DISTANCE = 5.0f;                            //起動までのプレイヤーとの距離
        constexpr float ALERT_DURATION_FALLBACK = 1.0f;                     //アラートの音の長さ(長さを読み込めなかったときのフォールバック)
        constexpr float ALERT_SOUND_RADIUS = 30.0f;                         //アラート音が聞こえる半径
        constexpr float EXPLOSION_FLASH_TIME_SCALE = 10.0f;                 //自爆前の点滅の速度
        constexpr int EXPLOSION_FLASH_MODULO = 2;                           //点滅の間隔の制御用
        constexpr Global::ColorRGB COLOR_NORMAL = Global::Palette::ORANGE;  //通常色
        constexpr Global::ColorRGB COLOR_EXPLODE = Global::Palette::RED;    //起爆時のアラート中の色
        constexpr Global::ColorRGB EXPLOSION_EFFECT_COLOR = Global::Palette::ORANGE_BRIGHT; //アラート中の色2
        constexpr float HIGHEST_Y_SENTINEL = -999.0f;                   //地面衝突判定の初期化数値
    }

    namespace Dummy {
        constexpr float TEXT_LIFETIME = 1.0f;               //ダメージ数値の表示されている時間
        constexpr float TEXT_HEIGHT_OFFSET = 0.5f;          //ダメージ数字の表示する頭上の高さ
        constexpr float ACCUMULATE_TIME_MAX = 0.2f;         //２つのヒットの数字を一つにまとめる時間の猶予

        constexpr Global::ColorRGB COLOR_NORMAL = Global::Palette::RED_LIGHT;   //ダミーの色
    }
}