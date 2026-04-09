#pragma once
#include "Param/Global.h"

namespace Item {

    namespace Weapon {
        constexpr float SPREAD_RANDOM_PRECISION = 1000.0f;      //拡散のランダム計算の精度
        constexpr float RAY_MAX_DISTANCE = 100.0f;              //弾道計算に使うレイの最大
        constexpr float RECOIL_RANDOM_PRECISION = 100.0f;       //リコイルのランダム計算の精度
        constexpr int SPREAD_RANDOM_RANGE = static_cast<int>(SPREAD_RANDOM_PRECISION * 2.0f);   //拡散計算の乱数幅
        constexpr float EQUIP_TIME = 0.15f;                     //武器の取り出しにかかる時間
        constexpr float ADS_SPEED = 0.6f;                       //構え状態の移行速度
        constexpr float RELOAD_END_SOUND_THRESHOLD = 0.25f;     //リロード完了音が鳴るタイミング（残り時間）
        constexpr float CROUCH_SPREAD_MULTIPLIER = 0.7f;        //しゃがみ時の拡散の軽減倍率
        constexpr int HEADSHOT_MULTIPLIER = 2;                  //ヘッドショット時のダメージ倍率

        constexpr float SOUND_RADIUS_NORMAL = 20.0f;            //リロードや空撃ちの音
        constexpr float SOUND_RADIUS_LOUD = 40.0f;              //射撃や爆発の音
    }

    namespace Item {
        constexpr int MAX_DROPPED = 10;         //マップ上に存在できるドロップアイテムの数
        constexpr float ITEM_PICKUP_RAD = 2.0f; //アイテムを拾える範囲
    }

    namespace Projectile {
        constexpr float TRAIL_RADIUS_MULTIPLIER = 1.0f;     //弾の軌跡の太さの倍率
    }
}