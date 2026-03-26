#pragma once
#include "Global.h"

namespace Chara {

    namespace Base {
        constexpr float CHARA_HEIGHT = 2.0f;
        constexpr float CHARA_RADIUS = 0.5f;

        constexpr float GRAVITY = -0.008f;
        constexpr float GROUND_KB_FRICTION = 0.8f;
        constexpr float AIR_KB_FRICTION = 0.98f;
        constexpr float GROUND_NORMAL_MIN = 0.3f;
        constexpr float WALL_NORMAL_MAX = 0.4f;
        constexpr float CEILING_NORMAL_MAX = -0.1f;

        constexpr int RAY_COUNT = 5;
        constexpr float STEP_RAY_START = 0.1f;
        constexpr float STEP_RAY_END = -0.2f;

        constexpr int SHADOW_ALPHA = 100;
        constexpr float SHADOW_OFFSET_Y_HIGH = 0.02f;
        constexpr float SHADOW_OFFSET_Y_LOW = 0.01f;
        constexpr float SHADOW_WIDTH_DIVISOR = 1.5f;
        constexpr int SHADOW_CONE_SEGMENTS = 16;

        constexpr int PARAM_MAX = 255;

        constexpr float HALF_RATIO = 0.5f;
        constexpr float CAP_BOTTOM_OFFSET = 0.3f;
        constexpr float CAP_SIDE_OFFSET = 0.8f;

        constexpr float MOVEMENT_MIN = 0.01f;
    }

    namespace Player {
        constexpr float CROUCH_ACCEL_RATE = 0.08f;
        constexpr float CROUCH_FRICTION_ADD = 0.08f;
        constexpr float SLIDE_MIN_SPEED = 0.18f;
        constexpr float SLIDE_BOOST = 2.5f;
        constexpr float SLIDE_COOLDOWN = 5.0f;
        constexpr float RUN_ACCEL_ADD = 0.02f;
        constexpr float RUN_FRICTION_SUB = 0.05f;
        constexpr float JUMP_POWER = 0.25f;
        constexpr float AIR_ACCEL_RATE = 0.1f;
        constexpr float FOV_LERP_RATE = 0.1f;
        constexpr float FOV_SPEED_BASE = 0.75f;
        constexpr float MAX_FOV = 110.0f * Global::Math::DEG_TO_RAD;
        constexpr float BOBBING_SPEED_MULT = 1.3f;
        constexpr float BOBBING_AMPLITUDE = 0.05f;
        constexpr float BOBBING_DECAY = 0.7f;
        constexpr float BOBBING_CROUCH_MIN_SPEED = 0.06f;
        constexpr float DROP_ITEM_Y_OFFSET = 0.4f;
        constexpr float STICK_INPUT_SCALE = 1000.0f;
        constexpr float INPUT_VECTOR_MAX_LENGTH = 1.0f;
        constexpr float YAW_HALF_TURN_DEG = 180.0f;
        constexpr float YAW_FULL_TURN_DEG = 360.0f;
        constexpr float ADS_SWAY_SCALE = 0.1f;
        constexpr float BOBBING_2D_AMPLITUDE = 0.015f;
        constexpr float BOBBING_2D_FREQUENCY_MULT = 0.5f;

        constexpr int MAX_WEAPON_SLOT_EASY_NORMAL = 10;
        constexpr int MAX_WEAPON_SLOT_HARD = 2;

        constexpr float SWAY_MULTIPLIER = 0.02f;
        constexpr float MAX_SWAY = 0.5f;
        constexpr float SWAY_LERP_SPEED = 0.2f;
    }

    namespace EnemyCommon {
        constexpr int PATH_UPDATE_RANDOM = 50;
        constexpr float PATH_NODE_REACHED_DIST = 1.0f;

        constexpr float PATH_SAFETY_CHECK_INTERVAL = 1.0f;
        constexpr float PATH_SAFETY_RAY_HEIGHT = 1.0f;
        constexpr float PATH_SAFETY_NORMAL_MIN = 0.6f;
        constexpr float PATH_SAFETY_HEIGHT_DIFF_MAX = 0.8f;
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
        constexpr int MAXENEMY_ONMAP = 15;
        constexpr int MAX_LIMIT_MELEE = 6;
        constexpr int MAX_LIMIT_RIFLE = 4;
        constexpr int MAX_LIMIT_SNIPER = 3;
        constexpr int MAX_LIMIT_ROLLING = 2;
        constexpr float INIT_SPAWN_INTERVAL = 8.0f;
        constexpr float PREPARE_TIME = 10.0f;
        constexpr float DIFFICULTY_UP_INTERVAL = 20.0f;
        constexpr float DIFFICULTY_UP_AMOUNT = 0.5f;
        constexpr float MIN_SPAWN_INTERVAL = 0.2f;
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
        constexpr float SWING_90_DEG = Global::Math::PI_HALF;        
        constexpr float SWING_QUARTER_TURN = Global::Math::PI_QUARTER;

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