#include "ItemManager.h"
#include "Player.h"
#include "Time.h"
#include "InputManager.h"
#include <algorithm>

ItemManager& ItemManager::GetIns() {
	static ItemManager ins;
	return ins;
}

void ItemManager::InitSpawners(const std::vector<VECTOR>& position) {
	spawners.clear();
	droppedItem.clear();

	std::vector<GunStatus>pool = {
		PLAYER_GUN::RIFLE,
		PLAYER_GUN::SNIPER,
		PLAYER_GUN::LAUNCHER,
		PLAYER_GUN::SMG,
	};

	for (int i = 0; i < 10; ++i) {
		int a = GetRand(3);
		int b = GetRand(3);
		std::swap(pool[a], pool[b]);
	}

	for (size_t i = 0; i < position.size() && i < pool.size(); ++i) {
		Spawner s;
		s.pos = position[i];
		s.spawnedSpec = pool[i];
		s.item = std::make_unique<WeaponItem>(s.pos, std::make_unique<Weapon>(s.spawnedSpec));
		s.respawnTimer = 0.0f;
		spawners.push_back(std::move(s));
	}
}

void ItemManager::SpawnDroppedItem(std::unique_ptr<WeaponItem> item) {
	if (!item)return;

	droppedItem.push_back(std::move(item));
	if (droppedItem.size() > maxDropped) {
		droppedItem.erase(droppedItem.begin());
	}
}

void ItemManager::Update(Player* player) {
	float dt = Time::GetIns().GetDelta();
	float pickUpRadius = 2.0f;
	bool interact = InputManager::GetIns().IsActionTrigger(ActionID::INTERACT);
	VECTOR pPos = player ? player->GetPos() : VGet(0.0f, 0.0f, 0.0f);

	currentNearItem = nullptr;
	float minDist = 2.0f;

	for (auto& item : droppedItem) {
		if (item && item->IsAlive()) {
			float dist = VSize(VSub(item->GetPos(), pPos));
			if (dist < minDist) {
				minDist = dist;
				currentNearItem = item.get();
			}
		}
	}

	for (auto& spawner : spawners) {
		if (spawner.item && spawner.item->IsAlive()) {
			float dist = VSize(VSub(spawner.item->GetPos(), pPos));
			if (dist < minDist) {
				minDist = dist;
				currentNearItem = spawner.item.get();
			}
		}
	}

	if (interact && currentNearItem && player) {
		std::unique_ptr<Weapon> w = currentNearItem->PickUp();
		player->AddWeapon(w);
	}

	for (int i = (int)droppedItem.size() - 1; i >= 0; i--) {
		droppedItem[i]->Update();

		if (!droppedItem[i]->IsAlive()) {
			droppedItem.erase(droppedItem.begin() + i);
		}
	}

	for (auto& spawner : spawners) {
		if (spawner.item) {
			spawner.item->Update();

			if (!spawner.item->IsAlive()) {
				spawner.item.reset();
				spawner.respawnTimer = spawner.MAX_RESPAWNTIME;
			}
		}
		else {
			spawner.respawnTimer -= dt;
			if (spawner.respawnTimer <= 0.0f) {
				spawner.item = std::make_unique<WeaponItem>(spawner.pos, std::make_unique<Weapon>(spawner.spawnedSpec));
			}
		}
	}
}


void ItemManager::Draw() {
	for (const auto& item : droppedItem) {
		if(item)item->Draw();
	}
	for (const auto& spawner : spawners) {
		if (spawner.item)spawner.item->Draw();
	}
}

void ItemManager::Clear() {
	droppedItem.clear();
	spawners.clear();
}