#pragma once
#include<vector>
#include<memory>
#include"Enemy.h"
#include "Pathfinding.h"
class Player;

/// <summary>
/// 敵全体を管理するマネージャークラス
/// </summary>
class EnemyManager
{
private:
	std::vector<VECTOR>spawnPoints;
	Player* target;
	int stageHandle;
	int maxEnemiesOnMap;
	float currentSpawnInterval;
	float minSpawnInterval;
	float spawnTimer;
	float difficultyTimer;

	float prepareTimer;

	int maxLimitMelee;
	int maxLimitRifle;
	int maxLimitSniper;
	int maxLimitRolling;

	std::vector<std::unique_ptr<Enemy>> enemies;	//敵の配列
	std::vector<Node> mapNode;		//マップのノード
	EnemyManager() = default;

	int CountEnemyType(ENEMYTYPE type);
public:
	static EnemyManager& GetIns();

	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="modelhandle">ステージのモデルハンドル</param>
	void Init(int modelhandle,Player* player);

	void AddSpawnPoint(VECTOR pos) { spawnPoints.push_back(pos);
	}
	/// <summary>
	/// 敵をスポーンさせる。
	/// </summary>
	/// <param name="enemy">敵の種類 CHARA_STATUS参照</param>
	void Spawn(std::unique_ptr<Enemy> enemy);

	/// <summary>
	/// 生存している敵を更新する
	/// </summary>
	int Update();

	/// <summary>
	/// 生存している敵を描画する
	/// </summary>
	void Draw();

	/// <summary>
	/// 敵を全消去する
	/// </summary>
	void Clear();

	std::vector<std::unique_ptr<Enemy>>& GetEnemies(){ return enemies; }



	/// <summary>
	/// 移動経路を計算する関数
	/// </summary>
	/// <param name="startPos">開始地点</param>
	/// <param name="goalPos">最終ゴール地点</param>
	/// <returns>次に向かうノードを返す</returns>
	std::vector<VECTOR> CalculatePath(VECTOR startPos, VECTOR goalPos);

	

	float GetPrepareTimer()const { return prepareTimer; }
};
