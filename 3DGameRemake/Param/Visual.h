#pragma once
#include "DxLib.h"

namespace VisualParam {

    namespace WeaponAnim {
        constexpr float ANIM_EQUIP_START_SCALE = 0.5f;
        constexpr float ANIM_EQUIP_OFFSET_Y = -0.6f;
        constexpr float ANIM_EQUIP_OFFSET_Z = -0.3f;
        constexpr float ANIM_EQUIP_ROT_X = DX_PI_F / 4.0f;

        constexpr float ANIM_RELOAD_SMG_PHASE1 = 0.15f;
        constexpr float ANIM_RELOAD_SMG_PHASE2 = 0.85f;
        constexpr float ANIM_RELOAD_SMG_WINDUP_Y = -0.1f;
        constexpr float ANIM_RELOAD_SMG_SPINS = 6.0f;
        constexpr float ANIM_RELOAD_SMG_PULLBACK_Z = -0.2f;
        constexpr float ANIM_RELOAD_SMG_OVERSHOOT_Y = 0.1f;
        constexpr float ANIM_RELOAD_SMG_OVERSHOOT_ROT_X = -DX_PI_F / 8.0f;

        constexpr float ANIM_RELOAD_LR_PHASE1 = 0.3f;
        constexpr float ANIM_RELOAD_LR_PHASE2 = 0.7f;
        constexpr float ANIM_RELOAD_LR_SHOULDER_Y = 0.8f;
        constexpr float ANIM_RELOAD_LR_SHOULDER_X = 0.3f;
        constexpr float ANIM_RELOAD_LR_SHOULDER_Z = -0.2f;
        constexpr float ANIM_RELOAD_LR_ROT_X = -DX_PI_F / 2.5f;

        constexpr float MODEL_BASE_ROTATION_Y = -DX_PI_F / 2.0f;
    }

    namespace ItemUI {
        constexpr int COLOR_ITEM_RING_R = 255;
        constexpr int COLOR_ITEM_RING_G = 200;
        constexpr int COLOR_ITEM_RING_B = 0;
        constexpr int RING_BLEND_ALPHA = 150;

        constexpr float OUTER_RING_BOB_SPEED_MULT = 1.5f;
        constexpr float OUTER_RING_BOB_AMP = 0.05f;
        constexpr float OUTER_RING_ROT_SPEED_MULT = -0.5f;
        constexpr float OUTER_RING_OFFSET_Y = -0.2f;

        constexpr int COLOR_FALLBACK_CUBE_R = 255;
        constexpr int COLOR_FALLBACK_CUBE_G = 255;
        constexpr int COLOR_FALLBACK_CUBE_B = 0;

        constexpr float UI_FADE_TRANSITION_RANGE = 5.0f;

        constexpr float BOBBING_SPEED = 2.0f;
        constexpr float BOBBING_AMPLITUDE = 0.2f;
        constexpr float BASE_HEIGHT_OFFSET = 0.5f;
        constexpr float CUBE_HALF_SIZE = 0.25f;

        constexpr float UI_DISPLAY_DISTANCE = 15.0f;
        constexpr int UI_OFFSET_X = -20;
        constexpr int UI_OFFSET_Y_AMMO = -40;
        constexpr int UI_OFFSET_Y_NAME = -20;

        constexpr int RING_SEGMENTS = 32;
        constexpr float RING_RADIUS_INNER = 0.6f;
        constexpr float RING_RADIUS_OUTER = 0.8f;
    }

    namespace HUD {
        constexpr int COLOR_HUD_WHITE_R = 255;
        constexpr int COLOR_HUD_WHITE_G = 255;
        constexpr int COLOR_HUD_WHITE_B = 255;

        constexpr int COLOR_HUD_RED_R = 255;
        constexpr int COLOR_HUD_RED_G = 50;
        constexpr int COLOR_HUD_RED_B = 50;

        constexpr int COLOR_HUD_YELLOW_R = 255;
        constexpr int COLOR_HUD_YELLOW_G = 200;
        constexpr int COLOR_HUD_YELLOW_B = 0;

        constexpr int COLOR_HP_BAR_BG_R = 40;
        constexpr int COLOR_HP_BAR_BG_G = 40;
        constexpr int COLOR_HP_BAR_BG_B = 40;

        constexpr int COLOR_RESERVE_AMMO_TEXT_R = 200;
        constexpr int COLOR_RESERVE_AMMO_TEXT_G = 200;
        constexpr int COLOR_RESERVE_AMMO_TEXT_B = 200;

        constexpr int COLOR_RELOAD_CIRCLE_BG_R = 80;
        constexpr int COLOR_RELOAD_CIRCLE_BG_G = 80;
        constexpr int COLOR_RELOAD_CIRCLE_BG_B = 80;

        constexpr int COLOR_DAMAGE_FLASH_R = 255;
        constexpr int COLOR_DAMAGE_FLASH_G = 0;
        constexpr int COLOR_DAMAGE_FLASH_B = 0;

        constexpr int TOP_INFO_TEXT_Y = 50;

        constexpr int CROSSHAIR_DOT_SIZE = 4;

        constexpr float HITMARK_DURATION = 0.2f;
        constexpr float HITMARK_EASE_OUT_POWER = 3.0f;
        constexpr int HITMARK_THICKNESS_NORMAL = 2;
        constexpr float HITMARK_LINE_LENGTH_NORMAL = 8.0f;
        constexpr float HITMARK_OFFSET_MULTIPLIER_NORMAL = 12.0f;
        constexpr int HITMARK_THICKNESS_KILL = 4;
        constexpr float HITMARK_LINE_LENGTH_KILL = 18.0f;
        constexpr float HITMARK_OFFSET_MULTIPLIER_KILL = 24.0f;
        constexpr int HITMARK_THICKNESS_HS = 4;
        constexpr float HITMARK_LINE_LENGTH_HS = 14.0f;
        constexpr float HITMARK_OFFSET_MULTIPLIER_HS = 20.0f;
        constexpr float HITMARK_BASE_OFFSET = 8.0f;

        constexpr int HP_BAR_WIDTH = 600;
        constexpr int HP_BAR_HEIGHT = 10;
        constexpr int HP_BAR_MARGIN_BOTTOM = 30;
        constexpr float HP_LOW_RATIO_THRESHOLD = 0.3f;

        constexpr int WEAPON_ICON_SIZE = 250;
        constexpr int WEAPON_UI_MARGIN_RIGHT = 30;
        constexpr int WEAPON_UI_MARGIN_BOTTOM = 20;
        constexpr int WEAPON_ICON_PANEL_ALPHA = 150;
        constexpr int WEAPON_TEXT_PADDING_X = 20;
        constexpr int WEAPON_TEXT_Y_OFFSET_FROM_ICON_BOTTOM = 100;
        constexpr int WEAPON_AMMO_TEXT_Y_OFFSET = 30;
        constexpr int WEAPON_RESERVE_TEXT_Y_OFFSET = 60;

        constexpr int RELOAD_CIRCLE_RADIUS = 128;
        constexpr int RELOAD_CIRCLE_SEGMENTS = 128;
        constexpr int RELOAD_CIRCLE_LINE_THICKNESS = 3;
        constexpr int RELOAD_TEXT_Y_OFFSET = 15;

        constexpr float INDICATOR_RADIUS = 100.0f;
        constexpr float INDICATOR_RADIUS_INCREASE = 80.0f;
        constexpr float INDICATOR_ARC_ANGLE_SCALE = DX_PI_F / 4.0f;
        constexpr float INDICATOR_ARC_ANGLE_OFFSET = DX_PI_F / 16.0f;
        constexpr int INDICATOR_SEGMENTS = 16;
        constexpr int INDICATOR_LINE_THICKNESS = 3;

        constexpr int PICKUP_ICON_SIZE = 100;
        constexpr int PICKUP_BOX_WIDTH = 350;
        constexpr int PICKUP_BOX_HEIGHT = 150;

        constexpr float FADE_DURATION = 0.3f;
        constexpr float DAMAGE_FLASH_EASE_POWER = 4.0f;
        constexpr int DAMAGE_FLASH_ALPHA_MAX = 180;
        constexpr float DAMAGE_FLASH_BASE_THICKNESS = 40.0f;
        constexpr float DAMAGE_FLASH_OFFSET_MAX = 60.0f;

        // HUD.cpp のピックアップUI関連
        constexpr int PICKUP_ICON_TEXT_GAP = 20;
        constexpr int PICKUP_PADDING_X = 25;
        constexpr int PICKUP_PADDING_Y = 15;
        constexpr int PICKUP_BOX_START_Y_OFFSET = 120;
        constexpr int PICKUP_PANEL_ALPHA = 150;
        constexpr int PICKUP_ICON_PANEL_ALPHA = 200;
        constexpr int PICKUP_TEXT_Y_OFFSET_FROM_ICON_CENTER = -25;
        constexpr int PICKUP_TEXT_Y_OFFSET_NO_ICON = 10;
        constexpr int PICKUP_SUBTEXT_Y_OFFSET = 30;
        constexpr float DAMAGE_FLASH_DURATION = 1.0f;
    }

    namespace Effect {
        constexpr int COLOR_EFFECT_MELEE_R = 255;
        constexpr int COLOR_EFFECT_MELEE_G = 255;
        constexpr int COLOR_EFFECT_MELEE_B = 0;

        constexpr int COLOR_EFFECT_RIFLE_R = 0;
        constexpr int COLOR_EFFECT_RIFLE_G = 0;
        constexpr int COLOR_EFFECT_RIFLE_B = 255;

        constexpr int COLOR_EFFECT_SNIPER_R = 0;
        constexpr int COLOR_EFFECT_SNIPER_G = 255;
        constexpr int COLOR_EFFECT_SNIPER_B = 0;

        constexpr int COLOR_EFFECT_ROLLING_R = 255;
        constexpr int COLOR_EFFECT_ROLLING_G = 165;
        constexpr int COLOR_EFFECT_ROLLING_B = 0;

        constexpr int COLOR_EFFECT_HIT_R = 255;
        constexpr int COLOR_EFFECT_HIT_G = 100;
        constexpr int COLOR_EFFECT_HIT_B = 0;

        constexpr int COLOR_EFFECT_SPAWN_R = 200;
        constexpr int COLOR_EFFECT_SPAWN_G = 100;
        constexpr int COLOR_EFFECT_SPAWN_B = 255;

        constexpr int COLOR_TRAIL_FRIENDLY_R = 180;
        constexpr int COLOR_TRAIL_FRIENDLY_G = 180;
        constexpr int COLOR_TRAIL_FRIENDLY_B = 180;
        constexpr int COLOR_TRAIL_ENEMY_R = 255;
        constexpr int COLOR_TRAIL_ENEMY_G = 0;
        constexpr int COLOR_TRAIL_ENEMY_B = 0;
        constexpr int COLOR_TRAIL_LAUNCHER_R = 255;
        constexpr int COLOR_TRAIL_LAUNCHER_G = 100;
        constexpr int COLOR_TRAIL_LAUNCHER_B = 0;
        constexpr int COLOR_EXPLOSION_R = 255;
        constexpr int COLOR_EXPLOSION_G = 100;
        constexpr int COLOR_EXPLOSION_B = 0;

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

        constexpr int PI_APPROX_INT = 314;
        constexpr float RANDOM_PERCENT_DIVISOR = 100.0f;

        constexpr float PARTICLE_SPEED_BASE = 0.1f;
        constexpr float PARTICLE_SPEED_RANDOM_MULT = 0.4f;

        constexpr float EFFECT_GRAVITY = 0.025f;
        constexpr float EFFECT_FLOOR_Y_LIMIT = -10.0f;

        constexpr float PARTICLE_BOUNCE_COEFFICIENT = -0.5f;
        constexpr float PARTICLE_FRICTION_GROUND_XZ = 0.7f;
        constexpr float PARTICLE_FRICTION_AIR_XZ = 0.98f;
        constexpr float PARTICLE_ROT_DECAY = 0.8f;

        constexpr int CONE_SEGMENTS_LOW = 3;
        constexpr int SPHERE_SEGMENTS_LOW = 8;
        constexpr int CAPSULE_SEGMENTS_TRAIL = 6;
        constexpr int CAPSULE_SEGMENTS_HITSCAN = 8;
    }

}