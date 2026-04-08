#pragma once
#include "Param/Global.h"

namespace Visual {

    namespace WeaponAnim {
        constexpr float ANIM_EQUIP_START_SCALE = 0.5f;      //武器だしアニメーションの開始
        constexpr float ANIM_EQUIP_OFFSET_Y = -0.6f;        //武器だしのYオフセット
        constexpr float ANIM_EQUIP_OFFSET_Z = -0.3f;        //武器だしのZオフセット
        constexpr float ANIM_EQUIP_ROT_X = Global::Math::MATH_PI_QUARTER;       //武器だしアニメーションのX回転量

        constexpr float ANIM_RELOAD_SMG_PHASE1 = 0.15f;     //ピストルのリロードのフェーズ１に入る時間
        constexpr float ANIM_RELOAD_SMG_PHASE2 = 0.85f;     //ピストルのリロードのフェーズ２に入る時間
        constexpr float ANIM_RELOAD_SMG_WINDUP_Y = -0.1f;   //ピストルリロードの振り上げオフセット
        constexpr float ANIM_RELOAD_SMG_SPINS = 6.0f;       //ピストルリロードの回転スピード
        constexpr float ANIM_RELOAD_SMG_PULLBACK_Z = -0.2f; //ピストルリロードのアクション時手元に引き寄せる
        constexpr float ANIM_RELOAD_SMG_OVERSHOOT_Y = 0.1f; //
        constexpr float ANIM_RELOAD_SMG_OVERSHOOT_ROT_X = -Global::Math::MATH_PI_EIGHTH;

        constexpr float ANIM_RELOAD_LR_PHASE1 = 0.3f;       //ランチャーのリロードのフェーズ１に入る時間
        constexpr float ANIM_RELOAD_LR_PHASE2 = 0.7f;       //ランチャーのリロードのフェーズ２に入る時間

      //リロード中のランチャーの位置
        constexpr float ANIM_RELOAD_LR_SHOULDER_X = 0.3f;
        constexpr float ANIM_RELOAD_LR_SHOULDER_Y = 0.8f;
        constexpr float ANIM_RELOAD_LR_SHOULDER_Z = -0.2f;

        constexpr float ANIM_RELOAD_LR_ROT_X = -Global::Math::PI_VALUE / 2.5f;  //リロード中のランチャーの回転角度
        constexpr float ANIM_RELOAD_DEFAULT_TRANSITION = 0.07f;     //リロードアニメーションの強さ
        constexpr float MODEL_BASE_ROTATION_Y = -Global::Math::MATH_PI_HALF;    //モデルのベースのY軸回転
    }

    namespace ItemUI {
        constexpr Global::ColorRGB COLOR_ITEM_RING = Global::Palette::AMBER;    //ドロップアイテムの目印の描画
        constexpr int RING_BLEND_ALPHA = 150;       //リングの透明度

        constexpr float OUTER_RING_BOB_SPEED_MULT = 1.5f;       //リングの上下揺れの速度
        constexpr float OUTER_RING_BOB_AMP = 0.05f;             //リングの上下揺れの強さ
        constexpr float OUTER_RING_ROT_SPEED_MULT = -0.5f;      //リングの回転の速度
        constexpr float OUTER_RING_OFFSET_Y = -0.2f;            //外周リングの高さオフセット

        constexpr Global::ColorRGB COLOR_FALLBACK_CUBE = Global::Palette::YELLOW;       //ドロップアイテムの武器モデルが読み込めなかったときの代替キューブの色

        constexpr float UI_FADE_TRANSITION_RANGE = 5.0f;            //アイテムのUIテキストがフェードする距離

        constexpr float BOBBING_SPEED = 2.0f;               //ドロップアイテムの上下揺れの速度
        constexpr float BOBBING_AMPLITUDE = 0.2f;           //ドロップアイテムの揺れの強さ
        constexpr float BASE_HEIGHT_OFFSET = 0.5f;          //基準高さのオフセット
        constexpr float CUBE_HALF_SIZE = 0.25f;             //代替キューブのサイズ

        constexpr float UI_DISPLAY_DISTANCE = 15.0f;        //UIテキストが表示される距離
        constexpr int UI_OFFSET_X = -20;                    //アイテムUIのXオフセット
        constexpr int UI_OFFSET_Y_AMMO = -40;               //弾情報のテキストのYオフセット
        constexpr int UI_OFFSET_Y_NAME = -20;               //武器名のテキストのYオフセット
        constexpr int PICKUP_BOX_HEIGHT_NO_ICON = 70;       //武器アイコンがなかった時のサイズ
        constexpr int RING_SEGMENTS = 32;                   //アイテムのリングの分割数
        constexpr float RING_RADIUS_INNER = 0.6f;           //内側リングの半径
        constexpr float RING_RADIUS_OUTER = 0.8f;           //外側リングの背景
    }

    namespace HUD {
        //HUD用色
        constexpr Global::ColorRGB COLOR_HUD_WHITE = Global::Palette::WHITE;
        constexpr Global::ColorRGB COLOR_HUD_RED = Global::Palette::RED_LIGHT;
        constexpr Global::ColorRGB COLOR_HUD_YELLOW = Global::Palette::AMBER;
        constexpr Global::ColorRGB COLOR_HP_BAR_BG = Global::Palette::GRAY_DARK;
        constexpr Global::ColorRGB COLOR_RESERVE_AMMO_TEXT = Global::Palette::GRAY_LIGHT;
        constexpr Global::ColorRGB COLOR_RELOAD_CIRCLE_BG = Global::Palette::GRAY_MEDIUM;
        constexpr Global::ColorRGB COLOR_DAMAGE_FLASH = Global::Palette::RED;

        constexpr int TOP_INFO_TEXT_Y = 50;     //スコアと準備タイマー用座標

        constexpr int CROSSHAIR_DOT_SIZE = 4;   //クロスヘアのサイズ

        constexpr float HITMARK_DURATION = 0.2f;    //ヒットマークの描画時間
        constexpr float HITMARK_EASE_OUT_POWER = 3.0f;  //ヒットマークが拡散する速さ
        constexpr int HITMARK_THICKNESS_NORMAL = 2;     //通常ヒットマークの線の太さ
        constexpr float HITMARK_LINE_LENGTH_NORMAL = 8.0f;  //通常ヒットマークの線の長さ
        constexpr float HITMARK_OFFSET_MULTIPLIER_NORMAL = 12.0f;   //通常ヒットマークのオフセット
        constexpr int HITMARK_THICKNESS_KILL = 4;       //キルヒットマークの太さ
        constexpr float HITMARK_LINE_LENGTH_KILL = 18.0f;   //キルヒットマークの長さ
        constexpr float HITMARK_OFFSET_MULTIPLIER_KILL = 24.0f; //キルヒットマークのオフセット
        constexpr int HITMARK_THICKNESS_HS = 4;         //ヘッドショットマークの太さ
        constexpr float HITMARK_LINE_LENGTH_HS = 14.0f; //ヘッドショットマークの長さ
        constexpr float HITMARK_OFFSET_MULTIPLIER_HS = 20.0f;   //ヘッドショットマークのオフセット
        constexpr float HITMARK_BASE_OFFSET = 8.0f;     //ヒットマークの基準オフセット

        constexpr int HP_BAR_WIDTH = 600;       //HPバーの長さ
        constexpr int HP_BAR_HEIGHT = 10;       //HPバーの高さ
        constexpr int HP_BAR_MARGIN_BOTTOM = 30;    //HPバーと画面下部の余裕
        constexpr float HP_LOW_RATIO_THRESHOLD = 0.3f;  //何割から低体力とするか(色が変わる)

        constexpr int WEAPON_ICON_SIZE = 250;       //武器のアイコンサイズ
        constexpr int WEAPON_UI_MARGIN_RIGHT = 30;  //武器UIの右画面端との猶予
        constexpr int WEAPON_UI_MARGIN_BOTTOM = 20; //下画面端との猶予
        constexpr int WEAPON_ICON_PANEL_ALPHA = 150;//武器アイコンの透明度
        constexpr int WEAPON_TEXT_X_OFFSET = 20;   //武器テキストのXオフセット
        constexpr int WEAPON_TEXT_Y_OFFSET_FROM_ICON_BOTTOM = 100;  //アイコン下部を基準とした武器テキストのYオフセット
        constexpr int WEAPON_AMMO_TEXT_Y_OFFSET = 30;       //武器の弾情報テキストのYオフセット
        constexpr int WEAPON_RESERVE_TEXT_Y_OFFSET = 60;    //武器の予備弾テキストのYオフセット

        constexpr int RELOAD_CIRCLE_RADIUS = 128;       //リロードHUDの半径
        constexpr int RELOAD_CIRCLE_SEGMENTS = 128;     //リロードHUDの円の分割数
        constexpr int RELOAD_CIRCLE_LINE_THICKNESS = 3; //リロードHUDの円の太さ
        constexpr int RELOAD_TEXT_Y_OFFSET = 15;        //リロード中の文字のYオフセット

        constexpr float INDICATOR_RADIUS = 100.0f;      //警告HUDの半径
        constexpr float INDICATOR_RADIUS_INCREASE = 80.0f;  //警告半径の最大半径
        constexpr float INDICATOR_ARC_ANGLE_SCALE = Global::Math::PI_VALUE / 4.0f;  //警告の弧の大きさ
        constexpr float INDICATOR_ARC_ANGLE_OFFSET = Global::Math::PI_VALUE / 16.0f;    //警告のオフセット
        constexpr int INDICATOR_SEGMENTS = 16;      //警告の弧の分割数
        constexpr int INDICATOR_LINE_THICKNESS = 3; //警告線の太さ
        constexpr int INDICATOR_ALPHA_MIN = 100;        //
        constexpr int INDICATOR_ALPHA_RANGE = 155;

        constexpr int PICKUP_ICON_SIZE = 100;       //拾うHUDのアイコンサイズ
        constexpr int PICKUP_BOX_WIDTH = 350;       //拾うHUDの枠の幅
        constexpr int PICKUP_BOX_HEIGHT = 150;      //拾うHUDの枠の高さ
        constexpr int PICKUP_ICON_TEXT_GAP = 20;
        constexpr int PICKUP_PADDING_X = 25;
        constexpr int PICKUP_PADDING_Y = 15;
        constexpr int PICKUP_BOX_START_Y_OFFSET = 120;
        constexpr int PICKUP_PANEL_ALPHA = 150;
        constexpr int PICKUP_ICON_PANEL_ALPHA = 200;
        constexpr int PICKUP_TEXT_Y_OFFSET_FROM_ICON_CENTER = -25;
        constexpr int PICKUP_TEXT_Y_OFFSET_NO_ICON = 10;
        constexpr int PICKUP_SUBTEXT_Y_OFFSET = 30;

        constexpr float FADE_DURATION = 0.3f;       //タイマーがスコアに切り替わるときのフェードの秒数
        constexpr float DAMAGE_FLASH_DURITION = 1.0f;   //ダメージエフェクトの秒数
        constexpr float DAMAGE_FLASH_EASE_POWER = 4.0f; //ダメージエフェクトの強さ
        constexpr int DAMAGE_FLASH_ALPHA_MAX = 180;     //ダメージエフェクトの透明度
        constexpr float DAMAGE_FLASH_BASE_THICKNESS = 40.0f;    //ダメージエフェクトの基準太さ
        constexpr float DAMAGE_FLASH_OFFSET_MAX = 60.0f;        //ダメージエフェクトのオフセット

        

        constexpr int TOP_TEXT_BUF_SIZE = 128;
    }

    namespace Effect {
        constexpr Global::ColorRGB COLOR_EFFECT_MELEE = Global::Palette::YELLOW;
        constexpr Global::ColorRGB COLOR_EFFECT_RIFLE = Global::Palette::BLUE;
        constexpr Global::ColorRGB COLOR_EFFECT_SNIPER = Global::Palette::GREEN;
        constexpr Global::ColorRGB COLOR_EFFECT_ROLLING = Global::Palette::ORANGE;
        constexpr Global::ColorRGB COLOR_EFFECT_HIT = Global::Palette::ORANGE_BRIGHT;
        constexpr Global::ColorRGB COLOR_EFFECT_SPAWN = Global::Palette::VIOLET;
        constexpr Global::ColorRGB COLOR_MUZZLE_FLASH = Global::Palette::YELLOW;
        constexpr Global::ColorRGB COLOR_TRAIL_FRIENDLY = Global::Palette::GRAY;
        constexpr Global::ColorRGB COLOR_TRAIL_ENEMY = Global::Palette::RED;
        constexpr Global::ColorRGB COLOR_TRAIL_LAUNCHER = Global::Palette::ORANGE_BRIGHT;
        constexpr Global::ColorRGB COLOR_EXPLOSION = Global::Palette::ORANGE_BRIGHT;

        constexpr Global::ColorRGB COLOR_EFFECT_WHITE = { 255, 255, 255 };
        constexpr float DEATH_VEL_Y_RANDOM_MULT = 0.3f;
        constexpr float DEATH_VEL_Y_BASE = 0.2f;
        constexpr float DEATH_LIFE_BASE = 1.0f;
        constexpr float DEATH_SIZE_BASE = 0.05f;
        constexpr int DEATH_SIZE_RANDOM_MULT = 10;
        constexpr float MUZZLE_FLASH_LIFE = 0.05f;
        constexpr float HIT_SPEED_RANDOM_MULT = 0.25f;
        constexpr float HIT_SPEED_BASE = 0.1f;
        constexpr float HIT_LIFE_BASE = 0.4f;
        constexpr int HIT_LIFE_RANDOM = 40;
        constexpr float HIT_SIZE_BASE = 0.04f;
        constexpr int HIT_SIZE_RANDOM = 3;
        constexpr float EXPLOSION_LIFE = 0.6f;
        constexpr float EXPLOSION_SOUND_RADIUS = 50.0f;
        constexpr float TRAIL_HITSCAN_LIFE = 0.25f;
        constexpr float TRAIL_PROJECTILE_LIFE = 0.12f;

        constexpr float MUZZLE_FLASH_BASE_SIZE = 0.5f;
        constexpr float MUZZLE_FLASH_TOP_SCALE = 4.0f;
        constexpr float MUZZLE_FLASH_BOTTOM_SCALE = 0.5f;
        constexpr float MUZZLE_FLASH_SPHERE_SCALE = 0.6f;
        constexpr int BASE_PARTICLE_COUNT_DEATH_MIN = 15;
        constexpr int BASE_PARTICLE_COUNT_DEATH_RAND = 5;
        constexpr int HIT_PARTICLE_COUNT_ENEMY = 12;
        constexpr int HIT_PARTICLE_COUNT_WALL = 6;
        constexpr int SPAWN_PARTICLE_COUNT = 4;
        constexpr int EXPLOSION_DEBRIS_COUNT_MIN = 15;
        constexpr int EXPLOSION_DEBRIS_COUNT_RAND = 10;

        constexpr float PARTICLE_SPEED_BASE = 0.1f;
        constexpr float PARTICLE_SPEED_RANDOM_MULT = 0.4f;

        constexpr float EFFECT_GRAVITY = 0.025f;
        constexpr float EFFECT_FLOOR_Y_LIMIT = Global::World::Y_MIN;

        constexpr float PARTICLE_BOUNCE_COEFFICIENT = -0.5f;
        constexpr float PARTICLE_FRICTION_GROUND_XZ = 0.7f;
        constexpr float PARTICLE_FRICTION_AIR_XZ = 0.98f;
        constexpr float PARTICLE_ROT_DECAY = 0.8f;

        constexpr int CONE_SEGMENTS_LOW = 3;
        constexpr int SPHERE_SEGMENTS_LOW = 8;
        constexpr int CAPSULE_SEGMENTS_TRAIL = 6;
        constexpr int CAPSULE_SEGMENTS_HITSCAN = 8;

        constexpr int ROT_AXIS_RAND_RANGE = 100;
        constexpr float ROT_AXIS_RAND_OFFSET = 50.0f;
        constexpr float STOP_VEL_Y_THRESHOLD = 0.05f;

        constexpr float SPAWN_SCAN_Y_MULT = 2.0f;
        constexpr float SPAWN_PROGRESS_HALF = 0.5f;
        constexpr int SPAWN_RAD_RAND_RANGE = 10;
        constexpr int SPAWN_POS_RAND_RANGE = 20;
        constexpr float SPAWN_POS_RAND_OFFSET = 10.0f;
        constexpr float SPAWN_VEL_XZ_DIVISOR = 500.0f;
        constexpr float SPAWN_VEL_Y_DIVISOR = 200.0f;
        constexpr float SPAWN_VEL_Y_OFFSET = 5.0f;
        constexpr float SPAWN_LIFE_BASE = 0.2f;
        constexpr int SPAWN_LIFE_RAND_RANGE = 20;
        constexpr float SPAWN_SIZE_BASE = 0.03f;
        constexpr int SPAWN_SIZE_RAND_RANGE = 3;
        constexpr float SPAWN_VEL_DECAY = 0.9f;

        constexpr float EXPLOSION_SPEED_BASE = 0.2f;
        constexpr float EXPLOSION_SPEED_RAND_MULT = 0.8f;
        constexpr float EXPLOSION_ROT_SPEED_BASE = 0.1f;
        constexpr float EXPLOSION_ROT_SPEED_RAND_MULT = 0.5f;
        constexpr float EXPLOSION_SIZE_BASE = 0.04f;
        constexpr int EXPLOSION_SIZE_RAND_RANGE = 6;
        constexpr int EXPLOSION_CORE_ALPHA_MAX = 150;
        constexpr float EXPLOSION_CORE_POW = 3.0f;

        constexpr float TRAIL_INNER_RAD_MULT = 0.3f;
    }

}