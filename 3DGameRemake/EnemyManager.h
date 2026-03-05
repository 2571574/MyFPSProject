#pragma once
#include<vector>
#include<memory>
#include"Enemy.h"
#include "Pathfinding.h"
/*敵の実体を管理するクラス*/
class EnemyManager
{
private:
	std::vector<std::unique_ptr<Enemy>> enemies;	//敵の配列
	EnemyManager() = default;		//コンストラクタ
	std::vector<Node> mapNode;
public:
	/// <summary>
	/// 唯一のインスタンスを取得
	/// </summary>
	/// <returns>唯一のインスタンス</returns>
	static EnemyManager& GetIns();

	void Init(int modelhandle);
	/// <summary>
	/// 敵をスポーンさせる。
	/// </summary>
	/// <param name="enemy">敵の種類 CHARA_STATUS参照</param>
	void Spawn(std::unique_ptr<Enemy> enemy, int stageHandle);

	/// <summary>
	/// 生存している敵を更新する
	/// </summary>
	void Update();

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


	int GetNearestNodeID(VECTOR pos);
	std::vector<int> CalculatePath(VECTOR startPos, VECTOR goalPos);

	VECTOR GetNodePosition(int nodeID);
};
