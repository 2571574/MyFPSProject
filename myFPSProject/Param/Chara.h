#pragma once
#include "Param/Global.h"

//キャラクター関係の定数
namespace Chara {

    namespace Base {
        constexpr float CHARA_HEIGHT = 2.0f;            //キャラのデフォルト身長
        constexpr float CHARA_RADIUS = 0.5f;            //キャラのデフォルト幅

        constexpr float GRAVITY = -0.008f;              //重力
        constexpr float GROUND_KB_FRICTION = 0.8f;      //デフォルト地上摩擦
        constexpr float AIR_KB_FRICTION = 0.98f;        //デフォルト空中摩擦
        constexpr float GROUND_NORMAL_MIN = 0.3f;       //床とみなす法線ベクトルの角度
        constexpr float CEILING_NORMAL_MAX = -0.1f;     //天井とみなす法線ベクトルの角度

        constexpr int RAY_COUNT = 5;                    //キャラの着地判定のレイの本数
        constexpr float STEP_RAY_START = 0.1f;          //着地判定始点
        constexpr float STEP_RAY_END = -0.2f;           //着地判定終点
        constexpr float CAP_SIDE_OFFSET = 0.8f;         //着地判定のレイの間隔
        constexpr float CAP_BOTTOM_OFFSET = 0.3f;       //壁判定のためのカプセル下

        constexpr int SHADOW_ALPHA = 100;               //影のα値
        constexpr float SHADOW_OFFSET_Y_HIGH = 0.02f;   //影のY座標オフセット
        constexpr float SHADOW_OFFSET_Y_LOW = 0.01f;    //影のY座標オフセット
        constexpr float SHADOW_WIDTH_DIVISOR = 1.5f;    //影の横幅
        constexpr int SHADOW_CONE_SEGMENTS = 16;        //影描画に使う円錐の頂点数              


        constexpr float MOVEMENT_MIN = 0.01f;           //移動とみなす最小数
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
        constexpr float STICK_INPUT_SCALE = 1000.0f;            //スティックの入力値の上限
        constexpr float INPUT_VECTOR = 1.0f;                    //入力による移動の方向ベクトルの量
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
        constexpr float PATH_SAFETY_HEIGHT_DIFF_MAX = 0.8f;     //
        constexpr float PATH_SAFETY_TOTAL_HEIGHT_DIFF = 1.0f;
        constexpr float FALL_DEATH_Y = Global::World::Y_MIN;
        constexpr float STEP_LENGTH = 2.5f;

        constexpr float PATH_UPDATE_BASE_INTERVAL = 0.5f;
        constexpr float PATH_SAFETY_SAMPLE_STEP = 1.0f;
        constexpr float FOOTSTEP_SOUND_RADIUS = 30.0f;

        constexpr float LEAN_FACTOR = 1.5f;
        constexpr float SPAWN_PROCESS_TIME = 1.5f;
    }

    namespace EnemyManager {
        constexpr float BURST_SPAWN_THRESHOLD = 0.5f;
        constexpr int NORMAL_SPAWNCOUNT = 3;
        constexpr float SPAWNDIST_PLAYER = 10.0f;
        constexpr float SPAWNDIST_ENEMY = 2.0f;
        constexpr float SPAWN_RAY_START = 5.0f;
        constexpr float SPAWN_RAY_END = Global::World::Y_MIN;
        constexpr float SPAWN_SOUND_RADIUS = 30.0f;
    }

    namespace Melee {
        constexpr float ATTACK_WINDUP_TIME = 0.3f;
        constexpr float ATTACK_RECOVERY_TIME = 0.5f;
        constexpr Global::ColorRGB COLOR_NORMAL = Global::Palette::YELLOW;
        constexpr float RECOVERY_PHASE1_RATIO = 0.3f;
        constexpr float RECOVERY_PHASE2_RATIO = 0.7f;
        constexpr float SWING_90_DEG = Global::Math::MATH_PI_HALF;
        constexpr float SWING_QUARTER_TURN = Global::Math::MATH_PI_QUARTER;

        constexpr float MOVE_STOP_RANGE_RATIO = 0.9f;
        constexpr float MELEE_ATTACK_SOUND_RADIUS = 20.0f;
    }

    namespace Rifle {
        constexpr float STOP_DISTANCE_RATIO = 0.6f;
        constexpr Global::ColorRGB COLOR_NORMAL = Global::Palette::BLUE;
    }

    namespace Sniper {
        constexpr float ATTACK_DISTANCE_RATIO = 0.8f;
        constexpr float ESCAPE_DISTANCE_RATIO = 0.4f;
        constexpr float ESCAPE_CHECK_DIST = 2.0f;
        constexpr float TARGET_TIME = 4.5f;
        constexpr float INDICATOR_PHASE1_PROGRESS_RATIO = 0.6f;
        constexpr float INDICATOR_PHASE2_PROGRESS_RATIO = 0.4f;
        constexpr int INDICATOR_OUTER_ALPHA_PHASE1_MAX = 150;
        constexpr float INDICATOR_OUTER_RADIUS_PHASE1 = 0.03f;
        constexpr float INDICATOR_INNER_RADIUS_PHASE1 = 0.01f;
        constexpr Global::ColorRGB INDICATOR_OUTER_COLOR_PHASE1 = Global::Palette::CYAN;
        constexpr int INDICATOR_OUTER_ALPHA_PHASE2 = 200;
        constexpr float INDICATOR_OUTER_RADIUS_PHASE2_BASE = 0.05f;
        constexpr float INDICATOR_OUTER_RADIUS_WAVE_AMPL = 0.02f;
        constexpr float INDICATOR_INNER_RADIUS_PHASE2 = 0.02f;
        constexpr float INDICATOR_WAVE_FREQ_MULT = 8.0f;
        constexpr Global::ColorRGB INDICATOR_OUTER_COLOR_PHASE2_BASE = Global::Palette::ORANGE_BRIGHT;
        constexpr int INDICATOR_CAPSULE_SEGMENTS = 8;

        constexpr float CHARGE_SOUND_RADIUS = 40.0f;
    }

    namespace Rolling {
        constexpr float EXPLODE_TIME = 2.0f;
        constexpr float TRIGGER_DISTANCE = 5.0f;
        constexpr float ALERT_DURATION_FALLBACK = 1.0f;
        constexpr float ALERT_SOUND_VOLUME = 30.0f;
        constexpr float EXPLOSION_FLASH_TIME_SCALE = 10.0f;
        constexpr int EXPLOSION_FLASH_MODULO = 2;
        constexpr Global::ColorRGB COLOR_NORMAL = Global::Palette::ORANGE;
        constexpr Global::ColorRGB COLOR_EXPLODE = Global::Palette::RED;
        constexpr Global::ColorRGB EXPLOSION_EFFECT_COLOR = Global::Palette::ORANGE_BRIGHT;
        constexpr float HIGHEST_Y_SENTINEL = -999.0f;
    }

    namespace Dummy {
        constexpr float TEXT_LIFETIME = 1.0f;
        constexpr float TEXT_HEIGHT_OFFSET = 0.5f;
        constexpr float ACCUMULATE_TIME_MAX = 0.2f;

        constexpr Global::ColorRGB COLOR_NORMAL = Global::Palette::RED_LIGHT;
        constexpr float HIT_FLASH_DURATION = 0.1f;
    }
}