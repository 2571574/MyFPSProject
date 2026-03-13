#include "ItemManager.h"
#include "Player.h"
#include "InputManager.h"
#include <algorithm>

ItemManager& ItemManager::GetIns() {
	static ItemManager ins;
	return ins;
}

void ItemManager::Spawn(std::unique_ptr<WeaponItem> item) {
	if (item) items.push_back(std::move(item));
}

void ItemManager::Update(Player* player) {
	float pickUpRadius = 2.0f;
	bool hasPickThisFrame = false;

	for (int i = items.size() - 1; i >= 0; i--) {
		items[i]->Update();

		if (player && items[i]->IsAlive() && !hasPickThisFrame) {
			float dist = VSize(VSub(player->GetPos(), items[i]->GetPos()));

			if (dist <= pickUpRadius) {
				if (InputManager::GetIns().IsActionTrigger(ActionID::INTERACT)) {
					std::unique_ptr<Weapon> pickWeapon = items[i]->PickUp();

					if (player->AddWeapon(pickWeapon)) {
						hasPickThisFrame = true;
					}
					else {
						items[i] = std::make_unique<WeaponItem>(items[i]->GetPos(), std::move(pickWeapon));
					}
				}
			}
		}
	}

	items.erase(
		std::remove_if(items.begin(), items.end(), [](const std::unique_ptr<WeaponItem>& item) {
			return !item->IsAlive();
			}
		),
		items.end());
}


void ItemManager::Draw() {
	for (const auto& item : items) {
		item->Draw();
	}
}

void ItemManager::Clear() {
	items.clear();
}