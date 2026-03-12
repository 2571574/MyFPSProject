#pragma once
#include <vector>
#include "status.h"
class Character;
class Player;
class EnemyManager;
struct HitInfo {
	Character* character = nullptr;
	bool isHeadShot = false;
};
class CollisionManager
{
private:
	CollisionManager() = default;
	std::vector<Character*> characters;

public:
	static CollisionManager& GetIns();

	void Register(Character* chara);
	void Unregister(Character* chara);
	void Update(Player* player, EnemyManager* enemymanager);

	HitInfo CheckHitScan(VECTOR start, VECTOR end, TEAMID shooterTeam);
	HitInfo CheckProjectile(VECTOR pos, VECTOR nextPos, float radius, TEAMID shooter);

	void ProcessExplotion(VECTOR hitPos, float radius, int damage, TEAMID shooter);
};

