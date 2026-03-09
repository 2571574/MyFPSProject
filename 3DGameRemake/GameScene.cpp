#include "GameScene.h"
GameScene::GameScene(SceneManager* manager):BaseScene(manager),player(VGet(0,15,0),&camera),stageHandle(-1){}

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
	EnemyManager::GetIns().Init(stageHandle);
	EnemyManager::GetIns().Spawn(std::make_unique<MeleeEnemy>(VGet(5, 10, 10),&player),stageHandle);
}

void GameScene::Update() {
	Time::GetIns().Update();    //時間の更新
	ProjectileManager::GetIns().Update();   //弾の更新
	Debug::Update();
	player.Update();            //プレイヤーを更新
	EnemyManager::GetIns().Update();    //敵の更新
}

void GameScene::Draw() {
	DrawSphere3D(VGet(0, 0, 0), 0.2f, 16, GetColor(255, 255, 0), GetColor(255, 255, 0), TRUE);
	MV1DrawModel(stageHandle);
	ProjectileManager::GetIns().Draw();     //弾の描画
	EnemyManager::GetIns().Draw();          //敵の描画
	player.Draw();                        //プレイヤーの描画
	Debug::Draw();
}