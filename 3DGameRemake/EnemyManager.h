#pragma once
#include "Enemy.h"
#include "Pathfinding.h"

#include <map>
#include <vector>
#include <memory>

class Player;

/// <summary>
/// 敵全体を管理するマネージャークラス
/// </summary>
class EnemyManager
{
private:
	std::vector<VECTOR>spawnPoints;	//スポーン位置
	Player* target;
	int stageHandle;
	std::vector<std::unique_ptr<Enemy>> enemies;	//敵の配列
	std::vector<Node> mapNode;		//マップのノード
	std::map<ENEMYTYPE, int>killCounts;

	float currentSpawnInterval;	//スポーンのインターバル
	float spawnTimer;			//スポーンタイマー
	float difficultyTimer;		//難易度上昇のタイマー
	float prepareTimer;			//スタート前タイマー
	int totalScore;

	/// <summary>
	/// 指定した種類の敵が何体生存中かカウントする
	/// </summary>
	/// <param name="type">敵の種類</param>
	/// <returns>数を返す</returns>
	int CountEnemyType(ENEMYTYPE type);

	EnemyManager() = default;

public:
	static EnemyManager& GetIns();

	/// <summary>
	/// スポーンポイントを追加する
	/// </summary>
	/// <param name="pos">ポイントの座標</param>
	void AddSpawnPoint(VECTOR pos) { spawnPoints.push_back(pos);}

	/// <summary>
	/// 敵をスポーンさせる。
	/// </summary>
	/// <param name="enemy">敵の種類 CHARA_STATUS参照</param>
	void Spawn(std::unique_ptr<Enemy> enemy);

	void Init(int modelhandle,Player* player);
	int Update();
	void Draw();

	/// <summary>
	/// 敵を全消去する
	/// </summary>
	void Clear();

	/// <summary>
	/// 移動経路を計算する関数
	/// </summary>
	/// <param name="startPos">開始地点</param>
	/// <param name="goalPos">最終ゴール地点</param>
	/// <returns>次に向かうノードを返す</returns>
	std::vector<VECTOR> CalculatePath(VECTOR startPos, VECTOR goalPos);

	

	std::vector<std::unique_ptr<Enemy>>& GetEnemies(){ return enemies; }
	float GetPrepareTimer()const { return prepareTimer; }
	int GetKillCount(ENEMYTYPE type)const {
		auto it = killCounts.find(type);
		return it != killCounts.end() ? it->second : 0;
	}
	Player* GetPlayer()const { return target; }
	void SetStageHandle(int handle) { stageHandle = handle; }

	int GetTotalScore()const { return totalScore; }
};
