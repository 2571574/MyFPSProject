#include "ItemManager.h"
#include "Player.h"
#include "Time.h"
#include "InputManager.h"
#include "SoundManager.h"
#include "Param/Global.h"
#include "Param/Item.h"

#include<random>
#include <algorithm>


ItemManager& ItemManager::GetIns() {
	static ItemManager ins;
	return ins;
}

GunStatus ItemManager::GetPlayerGunStatus(WeaponID id) {
	switch (id) {
	case WeaponID::AR: return PLAYER_GUN::RIFLE;
	case WeaponID::SR: return PLAYER_GUN::SNIPER;
	case WeaponID::LR: return PLAYER_GUN::LAUNCHER;
	case WeaponID::SMG: return PLAYER_GUN::SMG;
	case WeaponID::PIS: return PLAYER_GUN::PISTOL;
	default:break;
	}

	// 全武器のプール
	std::vector<GunStatus> pool = {
		PLAYER_GUN::RIFLE,
		PLAYER_GUN::SNIPER,
		PLAYER_GUN::LAUNCHER,
		PLAYER_GUN::SMG,
	};

	std::vector<WeaponID> activeWeapons;

	// スポナー上に配置されている武器
	for (const auto& spawner : spawners) {
		if (spawner.item && spawner.item->IsAlive()) {
			const GunStatus* spec = spawner.item->GetSpec();
			if (spec) activeWeapons.push_back(spec->id);
		}
	}

	for (const auto& item : droppedItem) {
		if (item && item->IsAlive()) {
			const GunStatus* spec = item->GetSpec();
			if (spec) activeWeapons.push_back(spec->id);
		}
	}

	std::vector<GunStatus> availablePool;
	for (const auto& gun : pool) {
		bool found = false;
		for (auto activeId : activeWeapons) {
			if (gun.id == activeId) {
				found = true;
				break;
			}
		}
		if (!found) {
			availablePool.push_back(gun);
		}
	}

	if (availablePool.empty()) {
		availablePool = pool;
	}

	int randomIndex = GetRand(static_cast<int>(availablePool.size()) - 1);
	return availablePool[randomIndex];
}

void ItemManager::InitSpawners(const std::vector<SpawnerSetup>& setup) {
	spawners.clear();
	droppedItem.clear();

	for (const auto& set : setup) {
		Spawner s;
		s.SpawnerInfo = set;
		s.spawnedSpec = GetPlayerGunStatus(set.weaponId);

		s.item = std::make_unique<WeaponItem>(s.SpawnerInfo.pos, std::make_unique<Weapon>(s.spawnedSpec));
		s.respawnTimer = set.respawnTime;
		spawners.push_back(std::move(s));
	}
}

void ItemManager::SpawnDroppedItem(std::unique_ptr<WeaponItem> item) {
	if (!item)return;

	droppedItem.push_back(std::move(item));
	if (droppedItem.size() > Item::Item::MAX_DROPPED) {
		//古いものから削除
		droppedItem.erase(droppedItem.begin());
	}
}

void ItemManager::Update(Player* player) {
	float dt = Time::GetIns().GetDelta();
	float pickUpRadius = Item::Item::ITEM_PICKUP_RAD;
	bool interact = InputManager::GetIns().IsActionTrigger(ActionID::INTERACT);
	VECTOR pPos = player ? player->GetPos() : VGet(0.0f, 0.0f, 0.0f);

	currentNearItem = nullptr;
	float minDist = pickUpRadius;

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

		currentNearItem = nullptr;
		SoundManager::GetIns().PlaySE("Resource/Sound/pick.ogg");
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
				spawner.respawnTimer = spawner.SpawnerInfo.respawnTime;
			}
		}
		else {
			spawner.respawnTimer -= dt;
			if (spawner.respawnTimer <= 0.0f) {
				spawner.spawnedSpec = GetPlayerGunStatus(spawner.SpawnerInfo.weaponId);
				spawner.item = std::make_unique<WeaponItem>(spawner.SpawnerInfo.pos, std::make_unique<Weapon>(spawner.spawnedSpec));
			}
		}
	}
}


void ItemManager::Draw() {
	for (const auto& item : droppedItem) {
		if (item)item->Draw();
	}
	for (const auto& spawner : spawners) {
		if (spawner.item)spawner.item->Draw();
	}
}

void ItemManager::Clear() {
	droppedItem.clear();
	spawners.clear();
}