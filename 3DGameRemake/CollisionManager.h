#pragma once
#include <vector>
#include "status.h"
class Character;
class Player;
class EnemyManager;
struct HitInfo {
	Character* character = nullptr;
	bool isHeadShot = false;
	bool isWallHit = false;
	VECTOR hitPos = VGet(0.0f, 0.0f, 0.0f);
};

class CollisionManager
{
private:
	CollisionManager() = default;
	std::vector<Character*> characters;

	int stageHandle = -1;

public:
	static CollisionManager& GetIns();

	void Register(Character* chara);
	void Unregister(Character* chara);
	void Update(Player* player, EnemyManager* enemymanager);

	HitInfo CheckHitScan(VECTOR start, VECTOR end, TEAMID shooterTeam);
	HitInfo CheckProjectile(VECTOR pos, VECTOR nextPos, float radius, TEAMID shooter);

	bool ProcessExplotion(VECTOR hitPos, float radius, int damage, float knockbackPower, bool distanceFallOff, TEAMID shooter,WeaponID id = WeaponID::UNKNOWN);

	void SetStageHandle(int handle) { stageHandle = handle; }
};

