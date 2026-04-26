#pragma once
#include "DxLib.h"
#include "Param/System.h"
#include <string>
#include <unordered_map>
#include <vector>

/// <summary>
/// サウンドを管理するマネージャークラス
/// </summary>
class SoundManager
{
private:
    SoundManager() = default;
    ~SoundManager() { Clear(); }

    int currentBGMHandle = -1;

    // SEの多重再生用構造体
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

    void PlayBGM(const std::string& path);
    void StopBGM();

    /// <summary>
    /// 通常SEを再生
    /// </summary>
    /// <param name="path">サウンドのファイルパス</param>
    void PlaySE(const std::string& path);

    /// <summary>
	/// SEをフェードアウトしながら再生
    /// </summary>
    /// <param name="path">サウンドのファイルパス</param>
    /// <param name="fadeTimeSec">フェードアウトする時間（秒）</param>
    void PlaySEWithFadeOut(const std::string& path, float fadeTimeSec);

    /// <summary>
    /// SEを3Dで再生
    /// </summary>
    /// <param name="path">サウンドのファイルパス</param>
    /// <param name="position">再生位置</param>
    /// <param name="radius">音の届く範囲</param>
    /// <returns>ハンドル</returns>
    int Play3DSE(const std::string& path, VECTOR position, float radius = 30.0f);

    void StopSE(const std::string& path);
    void StopSE(int handle);

    void PauseAll();
    void ResumeAll();
    void StopAll();


    /// <summary>
    /// 3Dサウンドのリスナー位置を更新する
    /// </summary>
    /// <param name="pos">位置</param>
    /// <param name="front">前方向ベクトル</param>
    /// <param name="up">上方向ベクトル</param>
    void UpdateListener(VECTOR pos, VECTOR front, VECTOR up);

    float GetSoundDuration(const std::string& path);
};
