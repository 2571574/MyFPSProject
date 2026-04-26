#pragma once
#include "status.h"

#include <vector>

class Character;
class Player;
class EnemyManager;

//弾のヒット判定
struct HitInfo {
	Character* character = nullptr;
	bool isHeadShot = false;
	bool isWallHit = false;
	VECTOR hitPos = VGet(0.0f, 0.0f, 0.0f);
	VECTOR hitNormal = VGet(0.0f, 1.0f, 0.0f);
};

/// <summary>
/// 弾とキャラ同士の当たり判定を管理するクラス
/// </summary>
class CollisionManager
{
private:
	std::vector<Character*> characters;		//現在生存しているキャラのリスト
	int stageHandle = -1;		//ステージのモデルハンドル

	CollisionManager() = default;

public:
	static CollisionManager& GetIns();
	
	/// <summary>
	/// リストに登録する。リスト内のキャラと判定を取る。
	/// </summary>
	/// <param name="chara">登録するキャラ</param>
	void Register(Character* chara);
	
	/// <summary>
	/// リストから削除する。
	/// </summary>
	/// <param name="chara">削除するキャラ</param>
	void Unregister(Character* chara);

	void Update();

	/// <summary>
	/// 即着弾タイプの弾の判定を取る
	/// </summary>
	/// <param name="start">開始地点</param>
	/// <param name="end">終了地点</param>
	/// <param name="shooterTeam">射手のID</param>
	/// <returns>ヒット判定</returns>
	HitInfo CheckHitScan(VECTOR start, VECTOR end, TEAMID shooterTeam);

	/// <summary>
	/// 弾速があるタイプの弾の判定を取る
	/// </summary>
	/// <param name="pos">現在地</param>
	/// <param name="nextPos">次のフレームの位置</param>
	/// <param name="radius">半径</param>
	/// <param name="shooter">射手のID</param>
	/// <returns>ヒット判定</returns>
	HitInfo CheckProjectile(VECTOR pos, VECTOR nextPos, float radius, TEAMID shooterTeam);

	/// <summary>
	/// 爆発の当たり判定を取る
	/// </summary>
	/// <param name="hitPos">着弾点(爆発位置)</param>
	/// <param name="radius">半径</param>
	/// <param name="damage">ダメージ</param>
	/// <param name="knockbackPower">ノックバックの威力</param>
	/// <param name="distanceFallOff">距離減衰</param>
	/// <param name="shooter">射手のID</param>
	/// <param name="id">武器ID</param>
	/// <returns>誰かに当たったか</returns>
	bool ProcessExplosion(VECTOR hitPos, float radius, int damage, float knockbackPower, bool distanceFallOff, TEAMID shooter, WeaponID id = WeaponID::UNKNOWN, bool friendlyFire = true);

	void SetStageHandle(int handle) { stageHandle = handle; }
};

