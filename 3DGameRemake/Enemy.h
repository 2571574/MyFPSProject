#pragma once
#include "Character.h"
#include "Debug.h"
class Player;

/// <summary>
/// 全ての敵の基底クラス
/// </summary>
class Enemy : public Character
{
protected:
	bool nowSpawned;
	float spawnedTimer;
	Player* target;	//攻撃対象のプレイヤーのポインタ
	int stageHandle = -1;	//ステージのモデルハンドル

	std::vector<VECTOR> currentPath;	//現在の経路
	int currentNodeID;		//次に向かうノードのID
	float pathUpdateTimer;	//経路更新のタイマー

	bool isDirectPathSafe;	//直接攻撃しても安全かどうか
	ENEMYTYPE type;
public:

	Enemy(VECTOR pos, CharacterStatus& status, Player* _target, ENEMYTYPE type);
	virtual ~Enemy(){}

	/// <summary>
	/// 更新処理
	/// </summary>
	virtual void Update() override = 0;

	/// <summary>
	/// 描画処理
	/// </summary>
	virtual void Draw() override = 0;


	/// <summary>
	/// 攻撃処理
	/// </summary>
	virtual void Action() = 0;

	/// <summary>
	/// 被弾処理
	/// </summary>
	/// <param name="damage">喰らったダメージ量</param>
	void OnHit(int damage,WeaponID id = WeaponID::UNKNOWN)override;

	void SetAlive(bool tag) { alive = tag; }
	void SetStageHandle(int handle) { stageHandle = handle; }
	float GetRadius() const { return status.width / 2.0f; }	

	void SetPath(const std::vector<VECTOR>& path) {
		currentPath = path;
		currentNodeID = 0;
	}

	bool HasPath() const {
		return !currentPath.empty() && currentNodeID < currentPath.size();
	}

	VECTOR GetNextNodeID()const;

	void AdvancePathIndex() {
		currentNodeID++;
	}

	//移動
	
	bool CheckLineSight(const Character* target,float height);

	bool CheckPathSafety(VECTOR targetPos);

	VECTOR UpdateNavigation(const Character* target, float dt);
	
	ENEMYTYPE GetType()const { return type; }

	bool CheckFall() { return position.y < -10.0f; }

	bool NowSpawned() { return nowSpawned; }
};

