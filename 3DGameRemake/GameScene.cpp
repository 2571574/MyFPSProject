#include "GameScene.h"
#include "CollisionManager.h"
#include "SceneManager.h"
#include "TitleScene.h"
#include "ItemManager.h"
#include "ResultScene.h"
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
	std::vector<VECTOR> spawnerPos = {
		VGet(25.0f, 0.4f, 25.0f),
		VGet(-25.0f, 0.4f, 25.0f),
		VGet(27.0f, 20.4f, -27.0f),
		VGet(-27.0f, 20.4f, -27.0f)
	};
	ItemManager::GetIns().InitSpawners(spawnerPos);
	if (manager->GetcurrentMode() == PlayMode::MODE_EASY) {
		ItemManager::GetIns().Clear();
	}
	score = 0;
}

void GameScene::Update() {
	Time::GetIns().Update();    //時間の更新
	Debug::Update();

	if (InputManager::GetIns().IsActionTrigger(ActionID::PAUSE)) {
		isPaused = !isPaused;
		pauseSelectNum = 0;
	}

	if (isPaused) {
		PauseUpdate();
		return;
	}
	player.Update();            //プレイヤーを更新
	score +=EnemyManager::GetIns().Update();    //敵の更新
	CollisionManager::GetIns().Update(&player, &EnemyManager::GetIns());
	ProjectileManager::GetIns().Update();   //弾の更新
	ItemManager::GetIns().Update(&player);

	if (player.GetHP() <= 0) {
		if (manager->GetcurrentMode() == PlayMode::MODE_TUTORIAL) {
			player.revive();
		}
		else {
			manager->SetScore(score);
			manager->SetAccuracy(player.GetShots(), player.GetHits(), player.GetHeadShot());
			manager->ChangeScene(std::make_unique<ResultScene>(manager));
		}
	}
}

void GameScene::PauseUpdate() {
	SetMousePoint(CENTER_X, CENTER_Y);
	if (InputManager::GetIns().IsActionTrigger(ActionID::MENU_UP)) {
		pauseSelectNum--;
		if (pauseSelectNum < 0) pauseSelectNum = PAUSE_MAX - 1;
	}
	if (InputManager::GetIns().IsActionTrigger(ActionID::MENU_DOWN)) {
		pauseSelectNum++;
		if (pauseSelectNum >= PAUSE_MAX) pauseSelectNum = 0;
	}

	if (InputManager::GetIns().IsActionTrigger(ActionID::MENU_SELECT)) {
		if (pauseSelectNum == RESUME) {
			isPaused = false;
		}
		else if (pauseSelectNum == RETURN_TITLE) {
			manager->ChangeScene(std::make_unique<TitleScene>(manager));
		}
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
	if (isPaused) {
		PauseDraw();
	}
}

void GameScene::PauseDraw() {
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
	DrawBox(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, GetColor(0, 0, 0), TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	DrawString(CENTER_X - 40, CENTER_Y - 100, "PAUSE", GetColor(255, 255, 255));

	int colorResume = (pauseSelectNum == RESUME) ? GetColor(255, 255, 0) : GetColor(255, 255, 255);
	int colorTitle = (pauseSelectNum == RETURN_TITLE) ? GetColor(255, 255, 0) : GetColor(255, 255, 255);

	DrawString(CENTER_X - 80, CENTER_Y + 40 * pauseSelectNum, ">", GetColor(255, 255, 255));
	DrawString(CENTER_X - 60, CENTER_Y, "Resume", colorResume);
	DrawString(CENTER_X - 60, CENTER_Y + 40, "Return Title", colorTitle);
}