#include "GameScene.h"
#include "CollisionManager.h"
#include "SceneManager.h"
#include "TitleScene.h"
#include "ItemManager.h"
GameScene::GameScene(SceneManager* manager):BaseScene(manager),player(VGet(0,15,0),&camera,manager->GetcurrentMode()),stageHandle(-1){}

GameScene::~GameScene() {
	if (stageHandle != -1) {
		MV1DeleteModel(stageHandle);
	}

	EnemyManager::GetIns().Clear();
	ProjectileManager::GetIns().Clear();
}


void GameScene::Init() {
	stageHandle = MV1LoadModel("Resource/ArenaV5.mv1");
	MV1SetPosition(stageHandle, VGet(0.0f, 0.0f, 0.0f));
	MV1SetScale(stageHandle, VGet(0.02f, 0.02f, 0.02f));
	MV1SetupCollInfo(stageHandle, -1, 8, 8, 8);
	player.SetStageHandle(stageHandle);
	CollisionManager::GetIns().SetStageHandle(stageHandle);
	EnemyManager::GetIns().Init(stageHandle,&player);
	auto item1 = std::make_unique<WeaponItem>(VGet(0.0f, 0.4f, 25.0f), std::make_unique<Weapon>(PLAYER_GUN::SNIPER));
	auto item2 = std::make_unique<WeaponItem>(VGet(3.0f, 0.4f, 25.0f), std::make_unique<Weapon>(PLAYER_GUN::RIFLE));
	auto item3 = std::make_unique<WeaponItem>(VGet(6.0f, 0.4f, 25.0f), std::make_unique<Weapon>(PLAYER_GUN::LAUNCHER));
	auto item4 = std::make_unique<WeaponItem>(VGet(-3.0f, 0.4f, 25.0f), std::make_unique<Weapon>(PLAYER_GUN::SMG));
	auto item5 = std::make_unique<WeaponItem>(VGet(-6.0f, 0.4f, 25.0f), std::make_unique<Weapon>(PLAYER_GUN::SNIPER));
	ItemManager::GetIns().Spawn(std::move(item1));
	ItemManager::GetIns().Spawn(std::move(item2));
	ItemManager::GetIns().Spawn(std::move(item3));
	ItemManager::GetIns().Spawn(std::move(item4));
	ItemManager::GetIns().Spawn(std::move(item5));
	currentScore = 0;
}

void GameScene::Update() {
	Time::GetIns().Update();    //時間の更新
	Debug::Update();
	player.Update();            //プレイヤーを更新
	currentScore += EnemyManager::GetIns().Update();    //敵の更新
	CollisionManager::GetIns().Update(&player, &EnemyManager::GetIns());
	ProjectileManager::GetIns().Update();   //弾の更新
	ItemManager::GetIns().Update(&player);

	if (player.GetHP() <= 0) {
		manager->ChangeScene(std::make_unique<TitleScene>(manager));
	}
}

void GameScene::Draw() {
	DrawSphere3D(VGet(0, 0, 0), 0.2f, 16, GetColor(255, 255, 0), GetColor(255, 255, 0), TRUE);
	MV1DrawModel(stageHandle);
	ProjectileManager::GetIns().Draw();     //弾の描画
	EnemyManager::GetIns().Draw();          //敵の描画
	player.Draw();                        //プレイヤーの描画

	ItemManager::GetIns().Draw();
	Debug::Draw();
}