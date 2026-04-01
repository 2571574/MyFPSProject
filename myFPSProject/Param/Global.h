#pragma once
#pragma once

//共通定数
namespace Global {

    namespace Math {
        constexpr float FPS_BASE = 60.0f;          //フレームレート

        constexpr float PI_VALUE = 3.14159265358979323846f;     //円周率

        constexpr float DEG_TO_RAD = PI_VALUE / 180.0f;         //ラジアンへの変換
        constexpr float RAD_TO_DEG = 180.0f / PI_VALUE;         //度数への変換

        // 円周率
        constexpr float MATH_PI_HALF = PI_VALUE / 2.0f;     // 90度
        constexpr float MATH_PI_QUARTER = PI_VALUE / 4.0f;     // 45度
        constexpr float MATH_PI_EIGHTH = PI_VALUE / 8.0f;     // 22.5度

        constexpr float MS_TO_SEC = 0.001f;            //ミリ秒

        constexpr int PERCENT_MAX = 100;               //割合上限
    }
    namespace World {
        constexpr float Y_MIN = -10.0f;                //ワールドのY下限
    }

    //色の構造体
    struct ColorRGB {
        int r, g, b;
    };

    //色
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