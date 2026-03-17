#pragma once
#include "WeaponItem.h"
#include "Camera.h"
#include "Status.h"

#include <memory>
#include <vector>

class Player;

//アイテムスポナーの情報
struct Spawner{
	VECTOR pos;
	GunStatus spawnedSpec;
	std::unique_ptr<WeaponItem>item;
	float respawnTimer = 0.0f;
};

/// <summary>
/// フィールド上のドロップアイテムとアイテムスポナーを管理するクラス
/// </summary>
class ItemManager
{
private:
	std::vector < std::unique_ptr<WeaponItem>> droppedItem;	//ドロップアイテム
	std::vector <Spawner>spawners;		//スポナー
	WeaponItem* currentNearItem = nullptr;	//最短のアイテム

	int stageHandle;
	VECTOR camPos;

	ItemManager() = default;
public:
	static ItemManager& GetIns();

	/// <summary>
	/// スポナーの初期化
	/// </summary>
	/// <param name="position">設置する座標</param>
	void InitSpawners(const std::vector<VECTOR>& position);

	/// <summary>
	/// アイテムをスポーンさせる
	/// </summary>
	/// <param name="item">スポーンさせるアイテム</param>
	void SpawnDroppedItem(std::unique_ptr<WeaponItem>item);


	void Update(Player* player);
	void Draw();
	void Clear();

	const std::vector<Spawner>& GetSpawners() const { return spawners; }
	WeaponItem* GetNearItem()const { return currentNearItem; }

	void SetStageHandle(int handle) { stageHandle = handle; }
	int GetStageHandle()const { return stageHandle; }

	void SetCamPos(VECTOR pos) { camPos = pos; }
	VECTOR GetCamPos()const { return camPos; }
};

