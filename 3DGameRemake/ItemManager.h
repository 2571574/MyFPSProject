#pragma once
#include <vector>
#include <memory>
#include "WeaponItem.h"
class Player;

class ItemManager
{
private:
	std::vector<std::unique_ptr<WeaponItem>>items;
	ItemManager() = default;

public:
	static ItemManager& GetIns();
	void Spawn(std::unique_ptr<WeaponItem>item);
	void Update(Player* player);
	void Draw();
	void Clear();
};

