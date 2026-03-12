#include "GameScene.h"
#include "CollisionManager.h"
#include "SceneManager.h"
#include "TitleScene.h"
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
	EnemyManager::GetIns().Init(stageHandle,&player);
	currentScore = 0;
}

void GameScene::Update() {
	Time::GetIns().Update();    //時間の更新
	Debug::Update();
	player.Update();            //プレイヤーを更新
	currentScore += EnemyManager::GetIns().Update();    //敵の更新

	CollisionManager::GetIns().Update(&player, &EnemyManager::GetIns());
	ProjectileManager::GetIns().Update();   //弾の更新

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
	Debug::Draw();
}