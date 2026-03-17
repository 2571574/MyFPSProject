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

	int maxEnemiesOnMap;		//マップ上の敵の最大数
	float currentSpawnInterval;	//スポーンのインターバル
	float minSpawnInterval;		//インターバルの最小
	float spawnTimer;
	float difficultyTimer;
	float prepareTimer;

	int maxLimitMelee;
	int maxLimitRifle;
	int maxLimitSniper;
	int maxLimitRolling;
	int CountEnemyType(ENEMYTYPE type);

	EnemyManager() = default;

public:
	static EnemyManager& GetIns();

	void AddSpawnPoint(VECTOR pos) { spawnPoints.push_back(pos);
	}
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
};
