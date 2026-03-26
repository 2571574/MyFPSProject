#pragma once
namespace ItemParam {

    namespace Weapon {
        constexpr float SPREAD_RANDOM_PRECISION = 1000.0f;
        constexpr float RAY_MAX_DISTANCE = 100.0f;
        constexpr float RECOIL_RANDOM_PRECISION = 100.0f;
        constexpr int SPREAD_RANDOM_RANGE = static_cast<int>(SPREAD_RANDOM_PRECISION * 2.0f);
        constexpr float EQUIP_TIME = 0.15f;
        constexpr float ADS_SPEED = 0.6f;
        constexpr float RELOAD_END_SOUND_THRESHOLD = 0.25f;
        constexpr float CROUCH_SPREAD_MULTIPLIER = 0.7f;
        constexpr float MUZZLE_FLASH_DURATION = 0.5f;
        constexpr int HEADSHOT_DAMAGE_MULTIPLIER = 2;

        constexpr float FPS_BASE = 60.0f;
        constexpr float RECOIL_HALF_DIVISOR = 2.0f;
        constexpr float SOUND_RADIUS_NORMAL = 20.0f;
        constexpr float SOUND_RADIUS_LOUD = 40.0f;
    }

    namespace Item {
        constexpr int MAX_DROPPED = 10;
        constexpr float ITEM_PICKUP_RAD = 2.0f;
    }

    namespace Projectile {
        constexpr float FPS_BASE = 60.0f;
        constexpr int HEADSHOT_MULTIPLIER = 2;
        constexpr float TRAIL_RADIUS_MULTIPLIER = 1.0f;
    }

}