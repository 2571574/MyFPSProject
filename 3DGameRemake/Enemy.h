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
	Player* target;	//攻撃対象のプレイヤーのポインタ
	int stageHandle = -1;	//ステージのモデルハンドル

	std::vector<VECTOR> currentPath;	//現在の経路
	int currentNodeID;		//次に向かうノードのID
	float pathUpdateTimer;	//経路更新のタイマー

	bool isDirectPathSafe;	//直接攻撃しても安全かどうか

public:

	Enemy(VECTOR pos, CharacterStatus& status,Player* _target) : Character(pos, status), target(_target), currentNodeID(0), pathUpdateTimer(0), isDirectPathSafe(false) {}
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
	virtual void OnHit(int damage);

	void SetStageHandle(int handle) { stageHandle = handle; }
	float GetRadius() const { return status.width; }	

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
	void ApplyMovement(VECTOR moveDir, float dt,float velRatio = 1.0f);
	void UpdateVelocity(VECTOR moveDir, float dt,float velRatio = 1.0f);
	void UpdatePhysics(float dt);

	bool CheckLineSight(const Character* target,float height);

	bool CheckPathSafety(VECTOR targetPos);

	VECTOR UpdateNavigation(const Character* target, float dt);
	
	


};

