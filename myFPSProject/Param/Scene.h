#pragma once
#include "Param/System.h"

namespace Scene {

    namespace Manager {
        constexpr int ALPHA_MAX_SCALE = 255;    //透明度の最大値
        constexpr float FADE_SPEED = 2.0f;      //フェードイン/アウトの速度
        constexpr int MAX_RECORD = 5;           //ランキングの記録上限
    }

    namespace Common {
        constexpr float STAGE_MODEL_SCALE = 0.02f;      //ステージモデルのサイズスケール
        constexpr int COLLISION_SETUP_DIV_NUM = 8;      //マップの判定の分割数

        constexpr int PAUSE_BG_ALPHA = 128;             //ポーズ中の背景の透明度

        constexpr int PAUSE_TITLE_OFFSET_X = -40;       //ポーズ画面の表題座標X   
        constexpr int PAUSE_TITLE_OFFSET_Y = -100;      //ポーズ画面の表題座標Y
        constexpr int PAUSE_CURSOR_OFFSET_X = -80;      //ポーズ画面のカーソルのXオフセット
        constexpr int PAUSE_ITEM_LINE_HEIGHT = 60;      //ポーズ画面の項目ごとの高さ
        constexpr int PAUSE_ITEM_OFFSET_X = -60;        //ポーズ画面の項目のXオフセット
        constexpr int HELP_TEXT_MARGIN_X = 50;          //操作表示テキストが見切れないための猶予 X
        constexpr int HELP_TEXT_MARGIN_Y = 30;          //操作表示テキストが見切れないための猶予 Y
    }

    namespace Title {
        constexpr int BG_ALPHA_DEFAULT = 100;           //基本の背景の暗さ
        constexpr int BG_ALPHA_DARK = 200;              //背景の暗さを濃くする
        constexpr float BG_SCROLL_X = 50.0f;            //背景のスクロールスピード                 
        constexpr int MENU_UNSELECTED_ALPHA = 40;       //メニュー画面の選んでいない項目の枠の透明度    
        constexpr int KEYCONFIG_HIGHLIGHT_ALPHA = 30;   //キーコンフィグ画面で選んでいる項目の透明度加算量

        constexpr float SLIDE_EASE_POWER = 3.0f;        //遷移時スライドしてくる速さ

        //タイトルロゴの座標
        constexpr int TITLE_LOGO_X1 = 800;
        constexpr int TITLE_LOGO_Y1 = 100;
        constexpr int TITLE_LOGO_X2 = 300;
        constexpr int TITLE_LOGO_Y2 = 600;

        constexpr int CREDIT_BASE_X = 150;              //クレジットの基準X座標
        constexpr int CREDIT_TITLE_Y = 100;             //クレジットの表題Y座標
        constexpr int CREDIT_START_Y = 200;             //クレジット表記の開始Y座標
        constexpr int CREDIT_LINE_HEIGHT = 45;          //クレジットの1列の高さ
        constexpr int CREDIT_SECTION_GAP = 30;          //グループごとの行間
        constexpr int CREDIT_LINK_OFFSET_X = 350;       //リンクのXオフセット
        constexpr int CREDIT_ITEM_INDENT_X = 40;        //ベースに加算する項目のX座標

        constexpr int SETTINGS_TITLE_Y = 100;           //設定の表題Y座標
        constexpr int SETTINGS_CURSOR_OFFSET_X = -20;   //設定のカーソルのXオフセット

        constexpr int KEYCONFIG_LINE_MARGIN_X = 200;    //キーコンフィグの区切り線が見切れないための猶予X
        constexpr int KEYCONFIG_LINE_Y = 220;           //区切り線のY座標
        constexpr int KEYCONFIG_LINE_THICKNESS = 2;     //区切り線の太さ
        constexpr int KEYCONFIG_CURSOR_OFFSET_X = -40;  //キーコンフィグのカーソルのXオフセット
        constexpr int KEYCONFIG_BG_OFFSET_X = -50;      //キーコンフィグの選択ハイライトの枠のXオフセット
        constexpr int KEYCONFIG_BG_OFFSET_Y = -5;       //選択ハイライトの枠のYオフセット
        constexpr int KEYCONFIG_BASE_X = 400;           //キーコンフィグの基準X
        constexpr int KEYCONFIG_BASE_Y = 250;           //キーコンフィグの基準Y
        constexpr int KEYCONFIG_COL_KB_X = 900;         //キーコンフィグのキーボード割り当てのテキスト座標X
        constexpr int KEYCONFIG_COL_PAD_X = 1400;       //コントローラー割り当てのテキスト座標Y
        constexpr int KEYCONFIG_TITLE_X = 150;          //キーコンフィグの表題のX座標
        constexpr int KEYCONFIG_HEADER_Y = 180;         //キーコンフィグのデバイスを表す文字のY座標
        constexpr int KEYCONFIG_LINE_HEIGHT = 50;       //キーコンフィグの項目ごとの高さ

        //設定の１段階ごとの変更幅
        constexpr float VOLUME_STEP = 0.05f;            //音量の上がり幅
        constexpr float MOUSE_SENS_STEP = 0.001f;       //マウス感度の調整幅
        constexpr float PAD_SENS_STEP = 0.0001f;        //コントローラー感度の調整幅

        //実際の数値を描画する上でのスケール
        constexpr float VOLUME_DISPLAY_SCALE = 100.0f;
        constexpr float MOUSE_SENS_DISPLAY_SCALE = 1000.0f;
        constexpr float PAD_SENS_DISPLAY_SCALE = 10000.0f;

        constexpr float INTRO_DURATION = 0.5f;      //最初のアニメーションの時間
        constexpr float SLIDE_START_X = -500.0f;    //スライドして入ってくる前のテキスト位置

        //メニュー画面のテキストの座標
        constexpr int MENU_BASE_X = 150;
        constexpr int MENU_BASE_Y = 200;
        constexpr int MENU_LINE_HEIGHT = 110;
        constexpr int MENU_SLANT_X = 40;

        //メニュー画面の選択肢の背景の座標
        constexpr int MENU_BG_WIDTH = 400;
        constexpr int MENU_BG_HEIGHT = 75;
        constexpr int MENU_BG_SLANT = 35;
        constexpr int MENU_BG_OFFSET_X = -80;
        constexpr int MENU_BG_OFFSET_Y = -10;

        //説明ログの座標
        constexpr int DIALOG_X1 = System::Window::CENTER_X + 50;
        constexpr int DIALOG_Y1 = System::Window::CENTER_Y + 100;
        constexpr int DIALOG_X2 = System::Window::WINDOW_WIDTH - 50;
        constexpr int DIALOG_Y2 = System::Window::WINDOW_HEIGHT - 100;
    }

    namespace Tutorial {
        namespace Boundary {
            //戦闘エリアに移動する座標
            constexpr float COMBAT_MIN_X = -22.0f;
            constexpr float COMBAT_MAX_X = 9.0f;
            constexpr float COMBAT_MIN_Z = -30.0f;
            constexpr float COMBAT_MAX_Z = -20.0f;

            //移動と戦闘エリアの境目の座標
            constexpr float MOVE_TO_COMBAT_MIN_X = 11.0f;
            constexpr float MOVE_TO_COMBAT_MAX_X = 30.0f;

            //戦闘エリアと射撃場の境目の座標
            constexpr float COMBAT_TO_FREE_RANGE_MIN_X = -30.0f;
            constexpr float COMBAT_TO_FREE_RANGE_MAX_X = -24.0f;

            constexpr float MOVE_TO_OTHER_MIN_Z = -20.0f;
            constexpr float MOVE_TO_OTHER_MAX_Z = -18.0f;
        }

        //チュートリアルでスポーンさせる敵の座標
        namespace Pos {
            constexpr float SPAWN_X = -18.0f;
            constexpr float SPAWN_Y = 1.0f;
            constexpr float SPAWN_Z = -24.0f;
        }

        //射撃場のターゲットの基準位置
        constexpr float TARGET_BASE_X = -26.0f;         //ターゲットの基準X座標
        constexpr float TARGET_INTERVAL_X = 15.0f;      //ターゲットのX間隔
		constexpr float TARGET_BASE_Z = -5.0f;          //ターゲットの基準Z座標
		constexpr float TARGET_INTERVAL_Z = 15.0f;      //ターゲットのZ間隔

        //ボタンとして使うダミーの基準位置
        constexpr float BUTTON_BASE_X = 28.0f;
        constexpr float BUTTON_INTERVAL_X = -5.0f;
        constexpr float BUTTON_BASE_Z = -28.0f;

        //ボタンのテキスト関係
        constexpr float BUTTON_UI_VISIBLE_DIST = 15.0f;
        constexpr float BUTTON_UI_FADE_START = 10.0f;
        constexpr float BUTTON_UI_FADE_RANGE = 5.0f;
        constexpr float BUTTON_UI_OFFSET_Y = 2.0f;
        constexpr int BUTTON_UI_TEXT_OFFSET_X = -40;

        constexpr float TUTORIAL_SPAWN_TIMER = 0.5f;    //チュートリアルの武器スポーン間隔

        //ダイアログの箱生成の座標
        constexpr int DIALOG_X2_POS = 350;
        constexpr int DIALOG_Y_POS = 250;
    }

    namespace Game {
        constexpr int DEATH_SCREEN_ALPHA = 50;      //死亡時画面のα値
        constexpr int RESULT_BG_BLUR_PIXEL = 16;    //リザルト画面背景にする画像の使用ピクセル幅
        constexpr int RESULT_BG_BLUR_STRENGTH = 1000;   //ぼかしの強さ(100で約1ピクセル)

        constexpr float DEATH_DURATION = 2.5f;      //死亡演出の秒数

        //敵のスポーン高さ
        constexpr float SPAWN_Y_GROUND = 2.0f;
        constexpr float SPAWN_Y_HIGH = 10.0f;
        constexpr float SPAWN_Y_TOP = 22.0f;
    }

    namespace Result {
        //リザルト背景の明るさ
        constexpr int RESULT_BG_BRIGHTNESS = 120;

        //リザルト画面の文字の座標
        constexpr int RESULT_TITLE_X = 150;
        constexpr int RESULT_TITLE_Y = 100;
        constexpr int RESULT_SCORE_OFFSET_X = -150;
        constexpr int RESULT_SCORE_Y = 200;

        constexpr int RESULT_COL_LEFT_X = 300;
        constexpr int RESULT_COL_RIGHT_X = 1100;
        constexpr int RESULT_ROW_TOP_Y = 300;
        constexpr int RESULT_ROW_BOTTOM_Y = 550;

        constexpr int RESULT_LINE_HEIGHT = 45;
        constexpr int RESULT_ITEM_INDENT_X = 40;

        constexpr int RESULT_HELP_BAR_HEIGHT = 50;

        constexpr float PERCENTAGE_MULTIPLIER = 100.0f;     //射撃精度を%表記にするための定数
    }
}