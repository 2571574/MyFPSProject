#pragma once
#include "Param/Global.h"

namespace Visual {

    namespace WeaponAnim {
        constexpr float ANIM_EQUIP_START_SCALE = 0.5f;
        constexpr float ANIM_EQUIP_OFFSET_Y = -0.6f;
        constexpr float ANIM_EQUIP_OFFSET_Z = -0.3f;
        constexpr float ANIM_EQUIP_ROT_X = Global::Math::MATH_PI_QUARTER;

        constexpr float ANIM_RELOAD_SMG_PHASE1 = 0.15f;
        constexpr float ANIM_RELOAD_SMG_PHASE2 = 0.85f;
        constexpr float ANIM_RELOAD_SMG_WINDUP_Y = -0.1f;
        constexpr float ANIM_RELOAD_SMG_SPINS = 6.0f;
        constexpr float ANIM_RELOAD_SMG_PULLBACK_Z = -0.2f;
        constexpr float ANIM_RELOAD_SMG_OVERSHOOT_Y = 0.1f;
        constexpr float ANIM_RELOAD_SMG_OVERSHOOT_ROT_X = -Global::Math::MATH_PI_EIGHTH;

        constexpr float ANIM_RELOAD_LR_PHASE1 = 0.3f;
        constexpr float ANIM_RELOAD_LR_PHASE2 = 0.7f;
        constexpr float ANIM_RELOAD_LR_SHOULDER_Y = 0.8f;
        constexpr float ANIM_RELOAD_LR_SHOULDER_X = 0.3f;
        constexpr float ANIM_RELOAD_LR_SHOULDER_Z = -0.2f;
        constexpr float ANIM_RELOAD_LR_ROT_X = -Global::Math::PI_VALUE / 2.5f;
        constexpr float ANIM_RELOAD_DEFAULT_TRANSITION = 0.07f;
        constexpr float MODEL_BASE_ROTATION_Y = -Global::Math::MATH_PI_HALF;
    }

    namespace ItemUI {
        constexpr Global::ColorRGB COLOR_ITEM_RING = Global::Palette::AMBER;
        constexpr int RING_BLEND_ALPHA = 150;

        constexpr float OUTER_RING_BOB_SPEED_MULT = 1.5f;
        constexpr float OUTER_RING_BOB_AMP = 0.05f;
        constexpr float OUTER_RING_ROT_SPEED_MULT = -0.5f;
        constexpr float OUTER_RING_OFFSET_Y = -0.2f;

        constexpr Global::ColorRGB COLOR_FALLBACK_CUBE = Global::Palette::YELLOW;

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
        constexpr Global::ColorRGB COLOR_HUD_WHITE = Global::Palette::WHITE;
        constexpr Global::ColorRGB COLOR_HUD_RED = Global::Palette::RED_LIGHT;
        constexpr Global::ColorRGB COLOR_HUD_YELLOW = Global::Palette::AMBER;
        constexpr Global::ColorRGB COLOR_HP_BAR_BG = Global::Palette::GRAY_DARK;
        constexpr Global::ColorRGB COLOR_RESERVE_AMMO_TEXT = Global::Palette::GRAY_LIGHT;
        constexpr Global::ColorRGB COLOR_RELOAD_CIRCLE_BG = Global::Palette::GRAY_MEDIUM;
        constexpr Global::ColorRGB COLOR_DAMAGE_FLASH = Global::Palette::RED;

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
        constexpr float INDICATOR_ARC_ANGLE_SCALE = Global::Math::PI_VALUE / 4.0f;
        constexpr float INDICATOR_ARC_ANGLE_OFFSET = Global::Math::PI_VALUE / 16.0f;
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

        constexpr int INDICATOR_ALPHA_MIN = 100;
        constexpr int INDICATOR_ALPHA_RANGE = 155;
        
        constexpr int TOP_TEXT_BUF_SIZE = 128;

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

        constexpr int PI_APPROX_INT = 314;
        constexpr float RANDOM_PERCENT_DIVISOR = 100.0f;

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
        constexpr int ALPHA_MAX = 255;
    }

}