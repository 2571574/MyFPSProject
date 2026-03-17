#pragma once
#include "BaseScene.h"
#include "Status.h"

#include <memory>
#include <vector>
#include<string>


struct GameResult {
	int currentScore = 0;
	int Shot = 0;
	int totalHit = 0;
	int totalHeadHit = 0;

void Clear() {
	currentScore = 0;
	Shot = 0;
	totalHit = 0;
	totalHeadHit = 0;
}
};

/// <summary>
///　全てのシーンを繋げるマネージャークラス
/// </summary>
class SceneManager
{
private:
	std::unique_ptr<BaseScene> currentScene;	//現在のシーン
	bool exitTag;		//終了フラグ
	PlayMode currentMode;	//現在の難易度

	GameResult lastResult;	//直近の結果
	std::vector<std::vector<int>> ranking;	
	const std::string RANKING_FILE = "ranking.dat";
	static constexpr int MAX_RECORD = 5;	//ランキングの最大
	
	void SaveRanking();
	void LoadRanking();
public:
	SceneManager();

	/// <summary>
	/// 次のシーンに切り替える
	/// </summary>
	/// <param name="nextscene">次のシーンのインスタンス</param>
	void ChangeScene(std::unique_ptr<BaseScene> nextscene);

	void Update();
	void Draw();

	/// <summary>
	/// リザルト用射撃精度をセットする
	/// </summary>
	/// <param name="shot">合計射撃回数</param>
	/// <param name="hit">合計ヒット回数</param>
	/// <param name="headShot">合計ヘッドショット回数</param>
	void SetAccuracy(int shot, int hit, int headShot) {
		lastResult.Shot = shot;
		lastResult.totalHit = hit;
		lastResult.totalHeadHit = headShot;
	}
	
	//スコアのセット
	void SetScore(int score);

	//リザルト関係
	const GameResult& GetResult()const { return lastResult; }
	const std::vector<int>& GetRanking()const { return ranking[currentMode]; }
	
	//難易度
	void SetCurrentMode(PlayMode mode) { currentMode = mode; }
	PlayMode GetcurrentMode()const { return currentMode; }
	
	//終了フラグ
	void SetExitTag(bool tag) { exitTag = tag; }
	bool GetExitTag() { return exitTag; }
};

