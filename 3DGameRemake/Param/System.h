#pragma once
#include "Param/Global.h"
#include "Param/Chara.h"

namespace System {

    namespace Window {
        constexpr int WINDOW_WIDTH = 1920;
        constexpr int WINDOW_HEIGHT = 1080;
        constexpr int WINDOW_COLORBIT = 32;
        constexpr int CENTER_X = WINDOW_WIDTH / 2;
        constexpr int CENTER_Y = WINDOW_HEIGHT / 2;
        constexpr int CIRCLE_DIVNUM = 16;
    }

    namespace Time {
        constexpr float MAX_DELTA_TIME = 0.1f;
    }

    namespace Camera {
        constexpr float CAM_ANGLESNAP_PITCH = 89.9f;
        constexpr float CAM_ANGLESNAP_YAW = 180.0f;
        constexpr float YAW_SNAP = 360.0f;
        constexpr float RECOIL_SPEED = 15.0f;
        constexpr float RECOVERY_SPEED = 3.0f;
        constexpr float CAMERA_NEAR_CLIP = 0.01f;
        constexpr float CAMERA_FAR_CLIP = 1000.0f;
    }

    namespace Pathfinding {
        constexpr float GRID_SIZE = 2.0f;
        constexpr float CHARA_HEIGHT = Chara::Base::CHARA_HEIGHT;
        constexpr float CHARA_RADIUS = Chara::Base::CHARA_RADIUS;
        constexpr float MAP_START_Y = 30.0f;
        constexpr float MAP_END_Y = Global::World::Y_MIN;
        constexpr float MAX_SLOPE_NORMAL = 0.6f;
        constexpr float MAP_MIN_X = -30.0f;
        constexpr float MAP_MAX_X = 30.0f;
        constexpr float MAP_MIN_Z = -30.0f;
        constexpr float MAP_MAX_Z = 30.0f;
        constexpr float RAY_Y_OFFSET = 1.0f;
        constexpr float GROUND_CHECK_DIST = 0.4f;

        constexpr int CIRCLE_CHECK_SEGMENTS = 8;
        constexpr float CAPSULE_BOTTOM_MARGIN = 0.1f;
        constexpr float NODE_PLACEMENT_OFFSET_Y = 0.1f;
        constexpr float NEXT_RAY_OFFSET_Y = -0.5f;
        constexpr float NODE_CONNECT_MAX_DIST_RATE = 1.8f;
        constexpr float WALKABLE_HEIGHT_TOLERANCE = 0.2f;
        constexpr float HALF_HEIGHT_RATE = 0.5f;
        constexpr float GROUND_CHECK_RAY_OFFSET_Y_TOP = 0.5f;
        constexpr float GROUND_CHECK_RAY_OFFSET_Y_BOTTOM = -0.5f;
        constexpr float MAX_DISTANCE_SQUARED = 99999.0f;

        constexpr int MIN_DIV_COUNT = 1;
    }

    namespace Sound {
        constexpr int MAX_DUPLICATE = 8;
        constexpr int MAX_VOLUME_SCALE = 255;
    }

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
        constexpr float MIN_HEAD_RAD = 0.15f;
        constexpr float HITSCAN_RAY_THICKNESS = 0.05f;
        constexpr float MIN_DIST_SQUARED = 0.01f;
        constexpr float EXPLODE_UPWARD_BIAS = 0.5f;
        constexpr float BODY_RADIUS_DIVISOR = 2.0f;
        constexpr float HEAD_RADIUS_DIVISOR = 2.0f;
        constexpr float MIN_NECK_HEIGHT_BODYRAD_MULT = 2.0f;
        constexpr float PUSH_SPLIT_RATIO_EQUAL_MASS = 0.5f;
        constexpr float HITS_POS_NORMALIZE_EPS = 1.0f;
        constexpr float ZERO = 0.0f;
        constexpr float ONE = 1.0f;
        constexpr int HITFLAG_TRUE = 1;
        constexpr int MIN_DAMAGE_ON_FALLOFF = 1;
        constexpr float DEFAULT_FALLBACK_DIR_Y = 1.0f;
    }

    namespace Input {
        constexpr float STICK_DEADZONE = 0.01f;
        constexpr int MOUSE_WHEEL_UP = 1301;
        constexpr int MOUSE_WHEEL_DOWN = 1302;
        constexpr int MAX_KEYCODES = 256;
    }

}