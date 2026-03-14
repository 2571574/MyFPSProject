#pragma once
#include <memory>
#include "BaseScene.h"
#include "Status.h"
#include <vector>
#include<string>

/// <summary>
///　全てのシーンを繋げるマネージャークラス
/// </summary>
class SceneManager
{
private:
	std::unique_ptr<BaseScene> currentScene;	//現在のシーン
	bool exitTag;		//終了タグ
	PlayMode currentMode;
	int currentScore;	//スコア
	std::vector<std::vector<int>> ranking;
	const std::string RANKING_FILE = "ranking.dat";

	int Shot = 0;
	int totalHit = 0;
	int totalHeadHit = 0;
	void SaveRanking();
	void LoadRanking();
public:
	SceneManager();

	/// <summary>
	/// 次のシーンに切り替える
	/// </summary>
	/// <param name="nextscene">次のシーンのインスタンス</param>
	void ChangeScene(std::unique_ptr<BaseScene> nextscene);

	/// <summary>
	/// 更新処理　現在のシーンの更新処理を呼び出す
	/// </summary>
	void Update();

	/// <summary>
	/// 描画処理　現在のシーンの描画処理を呼び出す
	/// </summary>
	void Draw();

	void SetAccuracy(int shot, int hit, int headShot) {
		Shot = shot;
		totalHit = hit;
		totalHeadHit = headShot;
	}
	int GetShots()const { return Shot; }
	int GetHits()const { return totalHit; }
	int GetHeadShot()const { return totalHeadHit; }
	void SetScore(int score);
	int GetScore()const { return currentScore; }
	const std::vector<int>& GetRanking()const { return ranking[currentMode]; }
	void SetPlayMode(PlayMode mode) { currentMode = mode; }
	PlayMode GetcurrentMode()const { return currentMode; }
	void SetExitTag(bool tag) { exitTag = tag; }
	bool GetExitTag() { return exitTag; }
};

