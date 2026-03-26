#pragma once
#include "DxLib.h"

namespace Global {

    // 算術・単位変換用の共通定数
    namespace Math {
        constexpr float FPS_BASE = 60.0f;
        constexpr float DEG_TO_RAD = DX_PI_F / 180.0f;
        constexpr float RAD_TO_DEG = 180.0f / DX_PI_F;

        // 円周率
        constexpr float PI_HALF = DX_PI_F / 2.0f;     // 90度
        constexpr float PI_QUARTER = DX_PI_F / 4.0f;  // 45度
        constexpr float PI_EIGHTH = DX_PI_F / 8.0f;   // 22.5度

        constexpr float MS_TO_SEC = 0.001f;
        constexpr float RATIO_FROM_PERCENT = 0.01f;

        constexpr int PERCENT_MAX = 100;
    }

    namespace World {
        constexpr float Y_MIN = -10.0f;
    }

    struct ColorRGB {
        int r, g, b;
    };

    namespace Palette {
        constexpr ColorRGB WHITE = { 255, 255, 255 };
        constexpr ColorRGB BLACK = { 0, 0, 0 };
        constexpr ColorRGB RED = { 255, 0, 0 };
        constexpr ColorRGB RED_LIGHT = { 255, 50, 50 };
        constexpr ColorRGB YELLOW = { 255, 255, 0 };
        constexpr ColorRGB AMBER = { 255, 200, 0 };
        constexpr ColorRGB ORANGE = { 255, 165, 0 };
        constexpr ColorRGB ORANGE_BRIGHT = { 255, 100, 0 };
        constexpr ColorRGB GREEN = { 0, 255, 0 };
        constexpr ColorRGB CYAN = { 0, 200, 255 };
        constexpr ColorRGB BLUE = { 0, 0, 255 };
        constexpr ColorRGB MAGENTA = { 255, 0, 255 };
        constexpr ColorRGB VIOLET = { 200, 100, 255 };
        constexpr ColorRGB GRAY_DARK = { 40, 40, 40 };
        constexpr ColorRGB GRAY_MEDIUM = { 80, 80, 80 };
        constexpr ColorRGB GRAY = { 180, 180, 180 };
        constexpr ColorRGB GRAY_LIGHT = { 200, 200, 200 };
    }
}