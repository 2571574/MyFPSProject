#pragma once
#include "DxLib.h"
#include "Param/System.h"
#include <string>
#include <unordered_map>
#include <vector>

/// <summary>
/// 音響（環境音・SE・3Dサウンド）を統合管理するクラス
/// </summary>
class SoundManager
{
private:
    SoundManager() = default;
    ~SoundManager() { Clear(); }

    int currentBGMHandle = -1;

    // SEの多重再生用プール構造体
    struct SEPool {
        int baseHandle = -1;
        std::vector<int> duplicateHandles;
    };
    std::unordered_map<std::string, SEPool> sePools;

    // フェードアウト処理中のSEを管理する構造体
    struct FadeSE {
        int handle;
        float timer;
        float maxTime;
    };
    std::vector<FadeSE> fadingSEs;

    bool isBGMPaused = false;
    std::vector<int> pausedSEHandles;

public:
    static SoundManager& GetIns();

    /// <summary>
    /// 毎フレーム呼び出し、音量の更新やフェードアウト処理を行う
    /// </summary>
    void Update();

    void Clear();

    // --- 環境音（BGM）制御 ---
    void PlayBGM(const std::string& path);
    void StopBGM();

    // --- SE制御 ---
    // UI音など、どこにいても同じ音量で聞こえる2Dサウンド
    void PlaySE(const std::string& path);

    // リザルト画面用：指定した秒数かけて音量がゼロになるSE
    void PlaySEWithFadeOut(const std::string& path, float fadeTimeSec);

    // 銃声や足音など：空間の指定座標から聞こえる3Dサウンド
    int Play3DSE(const std::string& path, VECTOR position, float radius = 30.0f);

    void StopSE(const std::string& path);
    void StopSE(int handle);

    void PauseAll();
    void ResumeAll();
    void StopAll();

    // --- リスナー制御 ---
    // プレイヤーの耳（カメラ）の位置と向きを更新する
    void UpdateListener(VECTOR pos, VECTOR front, VECTOR up);

    float GetSoundDuration(const std::string& path);
};
