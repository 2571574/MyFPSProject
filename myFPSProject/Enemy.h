#pragma once
#include "Character.h"

class Player;

/// <summary>
/// 全ての敵の基底クラス
/// </summary>
class Enemy : public Character
{
protected:
	ENEMYTYPE type;			//敵のタイプ

	bool nowSpawned;	//現在スポーン中
	float spawnedTimer;	//スポーン中用のタイマー
	
	Player* target;	//攻撃対象のプレイヤーのポインタ
	bool isDirectPathSafe;	//直接攻撃しても安全かどうか

	std::vector<VECTOR> currentPath;	//現在の経路
	int currentNodeID;		//次に向かうノードのID
	float pathUpdateTimer;	//経路更新のタイマー

	float moveDistance;		//移動距離のカウンター
	float onHitFlashTimer;	//被弾時の点滅用タイマー

	int stageHandle = -1;	//ステージのモデルハンドル

public:

	Enemy(VECTOR pos, const CharacterStatus& status, Player* _target, ENEMYTYPE type);
	virtual ~Enemy(){}

	virtual void Update() override = 0;
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
	ENEMYTYPE GetType()const { return type; }
	bool CheckFall()const;
	bool NowSpawned()const { return nowSpawned; }


	/// <summary>
	/// 移動先のノードIDをセットする
	/// </summary>
	/// <param name="path">移動先のノードの配列</param>
	void SetPath(const std::vector<VECTOR>& path) {
		currentPath = path;
		currentNodeID = 0;
	}

	bool HasPath() const {
		return !currentPath.empty() && currentNodeID < currentPath.size();
	}

	/// <summary>
	/// 移動先のノードの座標を取得する
	/// </summary>
	/// <returns>次のノードの座標</returns>
	VECTOR GetNextNodeID()const;
	
	/// <summary>
	/// 予定経路上のノードを１個進める
	/// </summary>
	void AdvancePathIndex() {
		currentNodeID++;
	}

	/// <summary>
	/// スポーン後の無敵時間をスキップ
	/// </summary>
	void SkipSpawnPhase() {
		nowSpawned = false;
		spawnedTimer = 0.0f;
	}
	
	/// <summary>
	/// 指定した高さでターゲットとの間の障害物の有無をチェック
	/// </summary>
	/// <param name="target">ターゲット側の高さ</param>
	/// <param name="height">自分の高さ</param>
	/// <returns>障害物がなければtrue</returns>
	bool CheckLineSight(const Character* target,float height)const;

	/// <summary>
	/// ターゲットまでの経路上に障害物や崖がないかチェック
	/// </summary>
	/// <param name="targetPos"></param>
	/// <returns>経路が安全であればtrue</returns>
	bool CheckPathSafety(VECTOR targetPos)const;

	/// <summary>
	/// ターゲットの座標から移動経路を計算する
	/// </summary>
	/// <param name="target">ターゲットとなるキャラ</param>
	/// <param name="dt">デルタタイム</param>
	/// <returns>次の移動先の座標</returns>
	VECTOR UpdateNavigation(const Character* target, float dt);

	/// <summary>
	/// カウンターに基づき足音を鳴らす
	/// </summary>
	void UpdateFootstep();
};

