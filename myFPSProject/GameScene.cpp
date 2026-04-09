#include "GameScene.h"
#include "CollisionManager.h"
#include "SceneManager.h"
#include "TitleScene.h"
#include "ItemManager.h"
#include "ResultScene.h"
#include "ResourceManager.h"
#include "EffectManager.h"
#include "SoundManager.h"
#include "Param/Global.h"
#include "Param/Scene.h"
#include "Param/System.h"

GameScene::GameScene(SceneManager* manager)
	: BaseScene(manager)
	, player(VGet(0.0f, 0.0f, 25.0f), &camera, manager->GetcurrentMode())
	, stageHandle(-1) 
	, isPaused(false)
	, pauseSelectNum(0)
	, score(0){}

GameScene::~GameScene() {
	EnemyManager::GetIns().Clear();
	ProjectileManager::GetIns().Clear();
	EffectManager::GetIns().Clear();
	if (monochromeHandle != -1) {
		DeleteGraph(monochromeHandle);
	}
}


void GameScene::Init()
{
	SetMouseDispFlag(FALSE);
	stageHandle = ResourceManager::GetIns().GetModel("Resource/Arena.mv1");
	MV1SetPosition(stageHandle, VGet(0.0f, 0.0f, 0.0f));
	MV1SetScale(stageHandle, VGet(Scene::Common::STAGE_MODEL_SCALE, Scene::Common::STAGE_MODEL_SCALE, Scene::Common::STAGE_MODEL_SCALE));
	MV1SetupCollInfo(stageHandle, -1, Scene::Common::COLLISION_SETUP_DIV_NUM, Scene::Common::COLLISION_SETUP_DIV_NUM, Scene::Common::COLLISION_SETUP_DIV_NUM);

	fontLarge = ResourceManager::GetIns().GetFont("Resource/Font/JetBrainsMono_36.dft");
	fontMedium = ResourceManager::GetIns().GetFont("Resource/Font/NotoSansJP_20.dft");

	player.SetStageHandle(stageHandle);
	CollisionManager::GetIns().SetStageHandle(stageHandle);
	ItemManager::GetIns().SetStageHandle(stageHandle);

	EnemyManager::GetIns().Init(stageHandle, &player);
	EnemyManager::GetIns().AddSpawnPoint(VGet(25.0f, Scene::Game::SPAWN_Y_GROUND, 25.0f));
	EnemyManager::GetIns().AddSpawnPoint(VGet(-25.0f, Scene::Game::SPAWN_Y_GROUND, 25.0f));
	EnemyManager::GetIns().AddSpawnPoint(VGet(27.0f, Scene::Game::SPAWN_Y_GROUND, -27.0f));
	EnemyManager::GetIns().AddSpawnPoint(VGet(-27.0f, Scene::Game::SPAWN_Y_GROUND, -27.0f));
	EnemyManager::GetIns().AddSpawnPoint(VGet(12.0f, Scene::Game::SPAWN_Y_HIGH, 12.0f));
	EnemyManager::GetIns().AddSpawnPoint(VGet(-12.0f, Scene::Game::SPAWN_Y_HIGH, -12.0f));
	EnemyManager::GetIns().AddSpawnPoint(VGet(-6.0f, Scene::Game::SPAWN_Y_HIGH, 12.0f));
	EnemyManager::GetIns().AddSpawnPoint(VGet(6.0f, Scene::Game::SPAWN_Y_HIGH, -12.0f));
	EnemyManager::GetIns().AddSpawnPoint(VGet(0.0f, Scene::Game::SPAWN_Y_TOP, -25.0f));
	EnemyManager::GetIns().AddSpawnPoint(VGet(27.0f, Scene::Game::SPAWN_Y_TOP, -27.0f));
	EnemyManager::GetIns().AddSpawnPoint(VGet(-27.0f, Scene::Game::SPAWN_Y_TOP, -27.0f));

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
	EffectManager::GetIns().Clear();
	monochromeHandle = MakeGraph(System::Window::WINDOW_WIDTH, System::Window::WINDOW_HEIGHT, FALSE);
	isDeadSequence = false;
	deathTimer = 0.0f;
	score = 0;
	isSceneChange = false;
	SoundManager::GetIns().PlayBGM("Resource/Sound/GameBGM.wav");
}


void GameScene::Update() {
	Debug::Update();

	if (!isDeadSequence) {
		if (InputManager::GetIns().IsActionTrigger(ActionID::PAUSE)) {
			isPaused = !isPaused;
			pauseSelectNum = 0;

			if (isPaused) {
				SoundManager::GetIns().PauseAll();
				SoundManager::GetIns().PlaySE("Resource/Sound/pause.ogg");
			}
			else {
				SoundManager::GetIns().ResumeAll();
				SoundManager::GetIns().PlaySE("Resource/Sound/pause.ogg");
			}
		}

		if (isPaused) {
			PauseUpdate();
			return;
		}
		player.Update();
	}
	score += EnemyManager::GetIns().Update();
	EffectManager::GetIns().Update();
	CollisionManager::GetIns().Update();
	ProjectileManager::GetIns().Update();
	ItemManager::GetIns().Update(&player);

	if (!isDeadSequence) {
		if (player.GetHP() <= 0) {
			manager->SetCauseOfDeath(player.GetLastHitWeapon());
			isDeadSequence = true;
			deathTimer = 0.0f;

			SoundManager::GetIns().StopBGM();
			SoundManager::GetIns().PlaySEWithFadeOut("Resource/Sound/death.wav", 6.0f);
		}
	}
	else {
		deathTimer += Time::GetIns().GetDelta();
		if (deathTimer >= Scene::Game::DEATH_DURATION && !isSceneChange) {
			reqTransition = true;
		}
	}
}

void GameScene::PauseUpdate() {
	SetMousePoint(System::Window::CENTER_X, System::Window::CENTER_Y);
	if (InputManager::GetIns().IsActionTrigger(ActionID::MENU_UP)) {
		SoundManager::GetIns().PlaySE("Resource/Sound/cursormove.ogg");
		pauseSelectNum--;
		if (pauseSelectNum < 0) pauseSelectNum = PAUSE_MAX - 1;
	}
	if (InputManager::GetIns().IsActionTrigger(ActionID::MENU_DOWN)) {
		SoundManager::GetIns().PlaySE("Resource/Sound/cursormove.ogg");
		pauseSelectNum++;
		if (pauseSelectNum >= PAUSE_MAX) pauseSelectNum = 0;
	}

	if (InputManager::GetIns().IsActionTrigger(ActionID::MENU_SELECT)) {
		SoundManager::GetIns().PlaySE("Resource/Sound/select.ogg");
		if (pauseSelectNum == RESUME) {
			isPaused = false;
			SoundManager::GetIns().ResumeAll();
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
	ProjectileManager::GetIns().Draw();
	EnemyManager::GetIns().Draw();
	EffectManager::GetIns().Draw();
	if (isDeadSequence) {
		GetDrawScreenGraph(0, 0, System::Window::WINDOW_WIDTH, System::Window::WINDOW_HEIGHT, monochromeHandle);
		GraphFilter(monochromeHandle, DX_GRAPH_FILTER_MONO, 0, 0);

		DrawGraph(0, 0, monochromeHandle, FALSE);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, Scene::Game::DEATH_SCREEN_ALPHA);
		DrawBox(0, 0, System::Window::WINDOW_WIDTH, System::Window::WINDOW_HEIGHT, GetColor(Global::Palette::BLACK.r, Global::Palette::BLACK.g, Global::Palette::BLACK.b), TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
	else {
		player.Draw();
	}
	Debug::Draw();
	if (isPaused) {
		PauseDraw();
	}
	if (reqTransition && !isSceneChange) {
		isSceneChange = true;

		int bgHandle = MakeGraph(System::Window::WINDOW_WIDTH, System::Window::WINDOW_HEIGHT, FALSE);
		GetDrawScreenGraph(0, 0, System::Window::WINDOW_WIDTH, System::Window::WINDOW_HEIGHT, bgHandle);
		GraphFilter(bgHandle, DX_GRAPH_FILTER_GAUSS, Scene::Game::RESULT_BG_BLUR_PIXEL, Scene::Game::RESULT_BG_BLUR_STRENGTH);

		manager->SetScore(score);
		manager->SetAccuracy(player.GetShots(), player.GetHits(), player.GetHeadShot());
		manager->ChangeScene(std::make_unique<ResultScene>(manager, bgHandle));
	}
}

void GameScene::PauseDraw() {
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, Scene::Common::PAUSE_BG_ALPHA);
	DrawBox(0, 0, System::Window::WINDOW_WIDTH, System::Window::WINDOW_HEIGHT, GetColor(Global::Palette::BLACK.r, Global::Palette::BLACK.g, Global::Palette::BLACK.b), TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	int yellow = GetColor(Global::Palette::YELLOW.r, Global::Palette::YELLOW.g, Global::Palette::YELLOW.b);
	int white = GetColor(Global::Palette::WHITE.r, Global::Palette::WHITE.g, Global::Palette::WHITE.b);

	DrawStringToHandle(System::Window::CENTER_X + Scene::Common::PAUSE_TITLE_OFFSET_X, System::Window::CENTER_Y + Scene::Common::PAUSE_TITLE_OFFSET_Y, "PAUSE", white, fontLarge);
	int colorResume = (pauseSelectNum == RESUME) ? yellow : white;
	int colorTitle = (pauseSelectNum == RETURN_TITLE) ? yellow : white;
	DrawStringToHandle(System::Window::CENTER_X + Scene::Common::PAUSE_CURSOR_OFFSET_X, System::Window::CENTER_Y + Scene::Common::PAUSE_ITEM_LINE_HEIGHT * pauseSelectNum, ">", yellow, fontMedium);
	DrawStringToHandle(System::Window::CENTER_X + Scene::Common::PAUSE_ITEM_OFFSET_X, System::Window::CENTER_Y, "Resume", colorResume, fontMedium);
	DrawStringToHandle(System::Window::CENTER_X + Scene::Common::PAUSE_ITEM_OFFSET_X, System::Window::CENTER_Y + Scene::Common::PAUSE_ITEM_LINE_HEIGHT, "Return Title", colorTitle, fontMedium);
}