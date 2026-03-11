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
	void Spawn(std::unique_ptr<Enemy> enemy, int stageHandle);

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

	//当たったか判定する関数


	/// <summary>
	/// 即着の弾の判定をチェックする。
	/// </summary>
	/// <param name="start">発射地点</param>
	/// <param name="end">射程の終点</param>
	/// <param name="shooterTeam">射手のチームID</param>
	/// <returns>当たった敵を返す</returns>
	Enemy* CheckHitScan(VECTOR start, VECTOR end, TEAMID shooterTeam);

	/// <summary>
	/// 弾速のある弾のヒット判定をチェックする。
	/// </summary>
	/// <param name="pos">弾の位置</param>
	/// <param name="nextpos">次のフレームの弾の位置</param>
	/// <param name="radius">弾の半径</param>
	/// <param name="shooterTeam">射手のチームID</param>
	/// <returns>当たった敵を返す</returns>
	Enemy* CheckProjectile(VECTOR pos, VECTOR nextpos, float radius, TEAMID shooterTeam);
	std::vector<std::unique_ptr<Enemy>>& GetEnemies(){ return enemies; }



	/// <summary>
	/// 移動経路を計算する関数
	/// </summary>
	/// <param name="startPos">開始地点</param>
	/// <param name="goalPos">最終ゴール地点</param>
	/// <returns>次に向かうノードを返す</returns>
	std::vector<VECTOR> CalculatePath(VECTOR startPos, VECTOR goalPos);

	
};
