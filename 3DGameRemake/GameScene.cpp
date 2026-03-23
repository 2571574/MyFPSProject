#include "GameScene.h"
#include "CollisionManager.h"
#include "SceneManager.h"
#include "TitleScene.h"
#include "ItemManager.h"
#include "ResultScene.h"
#include "ResourceManager.h"

namespace {
	constexpr float DEATH_DURATION = 2.5f;
}
GameScene::GameScene(SceneManager* manager)
	: BaseScene(manager)
	, player(VGet(0,0,25),&camera,manager->GetcurrentMode())
	, stageHandle(-1){}

GameScene::~GameScene() {
	EnemyManager::GetIns().Clear();
	ProjectileManager::GetIns().Clear();
}


void GameScene::Init() {
	stageHandle = ResourceManager::GetIns().GetModel("Resource/Arena.mv1");
	MV1SetPosition(stageHandle, VGet(0.0f, 0.0f, 0.0f));
	MV1SetScale(stageHandle, VGet(0.02f, 0.02f, 0.02f));
	MV1SetupCollInfo(stageHandle, -1, 8, 8, 8);

	player.SetStageHandle(stageHandle);
	CollisionManager::GetIns().SetStageHandle(stageHandle);
	ItemManager::GetIns().SetStageHandle(stageHandle);

	EnemyManager::GetIns().Init(stageHandle, &player);
	EnemyManager::GetIns().AddSpawnPoint(VGet(25.0f, 2.0f, 25.0f));
	EnemyManager::GetIns().AddSpawnPoint(VGet(-25.0f, 2.0f, 25.0f));
	EnemyManager::GetIns().AddSpawnPoint(VGet(27.0f, 2.0f, -27.0f));
	EnemyManager::GetIns().AddSpawnPoint(VGet(-27.0f, 2.0f, -27.0f));
	EnemyManager::GetIns().AddSpawnPoint(VGet(12.0f, 10.0f, 12.0f));
	EnemyManager::GetIns().AddSpawnPoint(VGet(-12.0f, 10.0f, -12.0f));
	EnemyManager::GetIns().AddSpawnPoint(VGet(-6.0f, 10.0f, 12.0f));
	EnemyManager::GetIns().AddSpawnPoint(VGet(6.0f, 10.0f, -12.0f));
	EnemyManager::GetIns().AddSpawnPoint(VGet(0.0f, 22.0f, -25.0f));
	EnemyManager::GetIns().AddSpawnPoint(VGet(27.0f, 22.0f, -27.0f));
	EnemyManager::GetIns().AddSpawnPoint(VGet(-27.0f, 22.0f, -27.0f));

	//武器スポナー
	std::vector<SpawnerSetup> spawnerSetups = {
		{VGet(25.0f, 0.4f, 25.0f),WeaponID::UNKNOWN},
		{VGet(-25.0f, 0.4f, 25.0f),WeaponID::UNKNOWN},
		{VGet(27.0f, 20.4f, -27.0f),WeaponID::UNKNOWN},
		{VGet(-27.0f, 20.4f, -27.0f), WeaponID::UNKNOWN}
	};
	ItemManager::GetIns().InitSpawners(spawnerSetups);
	if (manager->GetcurrentMode() == PlayMode::MODE_EASY) {
		ItemManager::GetIns().Clear();
	}
	monochromeHandle = MakeGraph(WINDOW_WIDTH, WINDOW_HEIGHT, FALSE);
	isDeadSequence = false;
	deathTimer = 0.0f;
	score = 0;
}

void GameScene::Update() {
	Debug::Update();




	if (!isDeadSequence) {
	if (InputManager::GetIns().IsActionTrigger(ActionID::PAUSE)) {
		isPaused = !isPaused;
		pauseSelectNum = 0;
	}

	if (isPaused) {
		PauseUpdate();
		return;
	}
		player.Update();
	}						//プレイヤーを更新
	score += EnemyManager::GetIns().Update();    //敵の更新
	CollisionManager::GetIns().Update();
	ProjectileManager::GetIns().Update();   //弾の更新
	ItemManager::GetIns().Update(&player);

	if (!isDeadSequence) {
		//死亡時のリザルト画面遷移
		if (player.GetHP() <= 0) {
			manager->SetCauseOfDeath(player.GetLastHitWeapon());
			isDeadSequence = true;
			deathTimer = 0.0f;
		}
	}
	else {
		deathTimer += Time::GetIns().GetDelta();
		if (deathTimer >= DEATH_DURATION) {
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
	if (!isDeadSequence) {
		ItemManager::GetIns().SetCamPos(camera.GetPos());
	}
		MV1DrawModel(stageHandle);
		ItemManager::GetIns().Draw();
		ProjectileManager::GetIns().Draw();     //弾の描画
		EnemyManager::GetIns().Draw();          //敵の描画
	if (isDeadSequence) {
		GetDrawScreenGraph(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, monochromeHandle);
		GraphFilter(monochromeHandle, DX_GRAPH_FILTER_MONO,0,0);

		DrawGraph(0, 0, monochromeHandle, FALSE);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 50);
		DrawBox(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, GetColor(0, 0, 0), true);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

		
	}
	else {
		player.Draw();                        //プレイヤーの描画
	}
	Debug::Draw();
	if (isPaused) {
		PauseDraw();
	}
}

void GameScene::PauseDraw() {
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
	DrawBox(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, GetColor(0, 0, 0), TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	const int yellow = GetColor(255, 255, 0);
	const int white = GetColor(255, 255, 255);

	DrawString(CENTER_X - 40, CENTER_Y - 100, "PAUSE", white);

	int colorResume = (pauseSelectNum == RESUME) ? yellow : white;
	int colorTitle = (pauseSelectNum == RETURN_TITLE) ? yellow : white;
	
	
	DrawString(CENTER_X - 80, CENTER_Y + 40 * pauseSelectNum, ">", white);
	DrawString(CENTER_X - 60, CENTER_Y, "Resume", colorResume);
	DrawString(CENTER_X - 60, CENTER_Y + 40, "Return Title", colorTitle);
}