#pragma once
#include "Character.h"
#include "Debug.h"

class Player;

/*敵の基底クラス、Characterの派生*/
class Enemy : public Character
{
protected:
	Player* target;
	int stageHandle = -1;

	std::vector<int> currentPath;
	int currentNodeIndex;
	float pathUpdateTimer;
public:
	Enemy(VECTOR pos, CharacterStatus& status,Player* _target) : Character(pos, status),target(_target){}
	virtual ~Enemy(){}

	virtual void Update() override = 0;		//更新
	virtual void Draw() override = 0;		//描画
	virtual void Action() = 0;				//攻撃

	float GetRadius() const { return status.width / 2.0f; }	//半径を得る

	//被弾処理
	virtual void OnHit(int damage) {
		TakeDamage(damage);
		Debug::Log("HIT");
	}

	void SetStageHandle(int handle) { stageHandle = handle; }

	void SetPath(const std::vector<int>& path) {
		currentPath = path;
		currentNodeIndex = 0;
	}

	int GetNextNodeID()const {
		if (currentPath.empty() || currentNodeIndex >= currentPath.size()) {
			return -1;
		}
		return currentPath[currentNodeIndex];
	}

	void AdvancePathIndex() {
		currentNodeIndex++;
	}
};

