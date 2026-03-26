#pragma once
#include "System.h"

namespace Scene {

    namespace Manager {
        constexpr int ALPHA_MAX_SCALE = 255;
        constexpr float FADE_SPEED = 2.0f;
        constexpr int MAX_RECORD = 5;
    }

    namespace Common {
        constexpr float STAGE_MODEL_SCALE = 0.02f;
        constexpr int COLLISION_SETUP_DIV_NUM = 8;

        constexpr int PAUSE_BG_ALPHA = 128;
        constexpr int PAUSE_TITLE_OFFSET_X = -40;
        constexpr int PAUSE_TITLE_OFFSET_Y = -100;
        constexpr int PAUSE_CURSOR_OFFSET_X = -80;
        constexpr int PAUSE_ITEM_LINE_HEIGHT = 60;
        constexpr int PAUSE_ITEM_OFFSET_X = -60;

        constexpr int HELP_TEXT_MARGIN_X = 50;
        constexpr int HELP_TEXT_MARGIN_Y = 30;
    }

    namespace Title {
        constexpr int BG_ALPHA_DEFAULT = 100;
        constexpr int BG_ALPHA_DARK = 200;
        constexpr float BG_SCROLL_X = 50.0f; 
        constexpr float BOB_SPEED = 10.0f;
        constexpr int MENU_UNSELECTED_ALPHA = 40;
        constexpr int KEYCONFIG_HIGHLIGHT_ALPHA = 30;

        constexpr float SLIDE_EASE_POWER = 3.0f;

        constexpr int TITLE_LOGO_OFFSET_X1 = 800;
        constexpr int TITLE_LOGO_Y1 = 100;
        constexpr int TITLE_LOGO_OFFSET_X2 = 300;
        constexpr int TITLE_LOGO_Y2 = 600;

        constexpr int CREDIT_BASE_X = 150;
        constexpr int CREDIT_TITLE_Y = 100;
        constexpr int CREDIT_START_Y = 200;
        constexpr int CREDIT_LINE_HEIGHT = 45;
        constexpr int CREDIT_SECTION_GAP = 30;
        constexpr int CREDIT_LINK_OFFSET_X = 350;
        constexpr int CREDIT_ITEM_INDENT_X = 40;

        constexpr int KEYCONFIG_BASE_X = 400;
        constexpr int KEYCONFIG_BASE_Y = 250;
        constexpr int KEYCONFIG_LINE_HEIGHT = 50;
        constexpr int KEYCONFIG_COL_KB_X = 900;
        constexpr int KEYCONFIG_COL_PAD_X = 1400;
        constexpr int KEYCONFIG_TITLE_X = 150;
        constexpr int KEYCONFIG_HEADER_Y = 180;
        constexpr int KEYCONFIG_LINE_Y = 220;

        constexpr float VOLUME_STEP = 0.05f;
        constexpr float MOUSE_SENS_STEP = 0.001f;
        constexpr float PAD_SENS_STEP = 0.0001f;

        constexpr float VOLUME_DISPLAY_SCALE = 100.0f;
        constexpr float MOUSE_SENS_DISPLAY_SCALE = 1000.0f;
        constexpr float PAD_SENS_DISPLAY_SCALE = 10000.0f;

        constexpr float INTRO_DURATION = 0.5f;
        constexpr float SLIDE_START_X = -500.0f;

        constexpr int MENU_BASE_X = 150;
        constexpr int MENU_BASE_Y = 200;
        constexpr int MENU_LINE_HEIGHT = 110;
        constexpr int MENU_SLANT_X = 40;

        constexpr int MENU_BG_WIDTH = 400;
        constexpr int MENU_BG_HEIGHT = 75;
        constexpr int MENU_BG_SLANT = 35;
        constexpr int MENU_BG_OFFSET_X = -80;
        constexpr int MENU_BG_OFFSET_Y = -10;

        constexpr int DIALOG_X1 = System::Window::CENTER_X + 50;
        constexpr int DIALOG_Y1 = System::Window::CENTER_Y + 100;
        constexpr int DIALOG_X2 = System::Window::WINDOW_WIDTH - 50;
        constexpr int DIALOG_Y2 = System::Window::WINDOW_HEIGHT - 100;
    }

    namespace Tutorial {
        namespace Boundary {
            constexpr float COMBAT_MIN_X = -22.0f;
            constexpr float COMBAT_MAX_X = 9.0f;
            constexpr float COMBAT_MIN_Z = -30.0f;
            constexpr float COMBAT_MAX_Z = -20.0f;

            constexpr float MOVE_TO_COMBAT_MIN_X = 11.0f;
            constexpr float MOVE_TO_COMBAT_MAX_X = 30.0f;
            constexpr float MOVE_TO_COMBAT_Z_LIMIT = -20.0f;

            constexpr float COMBAT_TO_MOVE_Z_LIMIT = -18.0f;

            constexpr float FREE_RANGE_MIN_X = -30.0f;
            constexpr float FREE_RANGE_MAX_X = -24.0f;
        }
        namespace Pos {
            constexpr float SPAWN_X = -18.0f;
            constexpr float SPAWN_Y = 1.0f;
            constexpr float SPAWN_Z = -24.0f;
        }
        constexpr float TARGET_BASE_X = -26.0f;
        constexpr float TARGET_INTERVAL_X = 15.0f;
        constexpr float TARGET_BASE_Z = -5.0f;
        constexpr float TARGET_INTERVAL_Z = 15.0f;

        constexpr float BUTTON_BASE_X = 28.0f;
        constexpr float BUTTON_INTERVAL_X = -5.0f;
        constexpr float BUTTON_BASE_Z = -28.0f;

        constexpr float BUTTON_UI_VISIBLE_DIST = 15.0f;
        constexpr float BUTTON_UI_FADE_START = 10.0f;
        constexpr float BUTTON_UI_FADE_RANGE = 5.0f;
        constexpr float BUTTON_UI_OFFSET_Y = 2.0f;
        constexpr int BUTTON_UI_TEXT_OFFSET_X = -40;

        constexpr float TUTORIAL_SPAWN_TIMER = 0.5f;
        constexpr int DIALOG_X2_POS = 350;
        constexpr int DIALOG_Y_POS = 250;
    }

    namespace Game {
        constexpr int DEATH_SCREEN_ALPHA = 50;
        constexpr int RESULT_BG_BLUR_PIXEL = 16;
        constexpr int RESULT_BG_BLUR_STRENGTH = 1000;

        constexpr float DEATH_DURATION = 2.5f;

        constexpr float SPAWN_Y_GROUND = 2.0f;
        constexpr float SPAWN_Y_HIGH = 10.0f;
        constexpr float SPAWN_Y_TOP = 22.0f;
    }

    namespace Result {
        constexpr int RESULT_BG_BRIGHTNESS = 120;

        constexpr int RESULT_TITLE_X = 150;
        constexpr int RESULT_TITLE_Y = 100;
        constexpr int RESULT_SCORE_Y = 200;

        constexpr int RESULT_COL_LEFT_X = 300;
        constexpr int RESULT_COL_RIGHT_X = 1100;
        constexpr int RESULT_ROW_TOP_Y = 300;
        constexpr int RESULT_ROW_BOTTOM_Y = 550;

        constexpr int RESULT_LINE_HEIGHT = 45;
        constexpr int RESULT_ITEM_INDENT_X = 40;

        constexpr float PERCENTAGE_MULTIPLIER = 100.0f;
        constexpr int RESULT_HELP_BAR_HEIGHT = 50;
    }
}