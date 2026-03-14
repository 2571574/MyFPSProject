#pragma once
#include <vector>
#include <memory>
#include "WeaponItem.h"
#include "Status.h"
class Player;

struct Spawner{
	VECTOR pos;
	GunStatus spawnedSpec;
	std::unique_ptr<WeaponItem>item;
	float respawnTimer = 0.0f;
	const float MAX_RESPAWNTIME = 30.0f;
};
class ItemManager
{
private:
	std::vector < std::unique_ptr<WeaponItem>> droppedItem;
	std::vector <Spawner>spawners;

	size_t maxDropped = 10;
	WeaponItem* currentNearItem = nullptr;
	ItemManager() = default;

public:
	static ItemManager& GetIns();
	void InitSpawners(const std::vector<VECTOR>& position);
	void SpawnDroppedItem(std::unique_ptr<WeaponItem>item);
	void Update(Player* player);
	void Draw();
	void Clear();

	const std::vector<Spawner>& GetSpawners() const { return spawners; }
	WeaponItem* GetNearItem()const { return currentNearItem; }
};

