#include "TutorialScene.h"
#include "TitleScene.h"
#include "InputManager.h"
#include "Time.h"
#include "CollisionManager.h"
#include "ItemManager.h"
#include "EnemyManager.h"
#include "ProjectileManager.h"
#include "MeleeEnemy.h"
#include "RifleEnemy.h"
#include "SniperEnemy.h"
#include "RollingEnemy.h"
#include "ResourceManager.h"

namespace {
	constexpr float TUTORIAL_SPAWN_TIMER = 0.5f;
}
TutorialScene::TutorialScene(SceneManager* manager)
	:BaseScene(manager)
	, player(VGet(20.0f, 0.0f, 28.0f), &camera, PlayMode::MODE_TUTORIAL)
	, stageHandle(-1)
	, isPaused(false)
	, pauseSelectNum(0) {
}

TutorialScene::~TutorialScene() {
	EnemyManager::GetIns().Clear();
	ProjectileManager::GetIns().Clear();
	ItemManager::GetIns().Clear();
} 

void TutorialScene::Init() {
	currentPhase = TutorialPhase::MOVEMENT;
	currentEnemyInfo = -1;

	stageHandle = ResourceManager::GetIns().GetModel("Resource/TutorialArena.mv1");

	//ステージセット
	MV1SetPosition(stageHandle, VGet(0.0f, 0.0f, 0.0f));
	MV1SetScale(stageHandle, VGet(0.02f, 0.02f, 0.02f));
	MV1SetupCollInfo(stageHandle, -1, 8, 8, 8);

	player.SetStageHandle(stageHandle);
	CollisionManager::GetIns().SetStageHandle(stageHandle);
	EnemyManager::GetIns().SetStageHandle(stageHandle);
	ItemManager::GetIns().SetStageHandle(stageHandle);

	//武器スポナー
	std::vector<SpawnerSetup>spawnerSetups = {
		{VGet(-15.0f,0.4f,-16.0f),WeaponID::AR,TUTORIAL_SPAWN_TIMER},
		{VGet(-10.0f,0.4f,-16.0f),WeaponID::SR,TUTORIAL_SPAWN_TIMER},
		{VGet(0.0f,0.4f,-16.0f),WeaponID::SMG,TUTORIAL_SPAWN_TIMER},
		{VGet(5.0f,0.4f,-16.0f),WeaponID::LR,TUTORIAL_SPAWN_TIMER}
	};
	ItemManager::GetIns().InitSpawners(spawnerSetups);

	//的ダミー
	for (int i = 0; i < 3; ++i) {
		auto Target = std::make_unique<Dummy>(VGet(-26.0f + i * 15.0f, 0.0f, -5.0f + i * 15.0f), &player, true);
		target.push_back(std::move(Target));
	}

	//ボタンダミー 
	for (int i = 0; i < 4; i++) {
		SpawnButton btn;
		btn.pos = VGet(28.0f + i * -5.0f, 0.0f, -28.0f);
		btn.dummy = std::make_unique<Dummy>(btn.pos,&player,false);
		btn.enemyType = i;
		btn.spawnCT = 0.0f;
		button.push_back(std::move(btn));
	}
}

void TutorialScene::Update() {
	Debug::Update();
	float dt = Time::GetIns().GetDelta();

	//ポーズ処理
	if (InputManager::GetIns().IsActionTrigger(ActionID::PAUSE)) {
		isPaused = !isPaused;
		pauseSelectNum = 0;
	}
	if (isPaused) {
		PauseUpdate();
		return;
	}

	player.Update();

	//ダミー更新処理
	for (auto& btn : button) {
		if (btn.spawnCT > 0.0f)
			btn.spawnCT -= dt;
		if (btn.dummy)btn.dummy->Update();
	}
	for (auto& t : target) {
		if (t) {
			t->Update();
			if (t->GetHP() <= 0) t->revive();
		}
	}


	VECTOR pPos = player.GetPos();

	//ステート更新箇所の座標
	bool isPlayerCombatArea = (pPos.x >= -22.0f && pPos.x <= 9.0f && pPos.z >= -30.0f && pPos.z <= -20.0f);
	bool MovementToCombat = (pPos.x >= 11.0f && pPos.x <= 30.0f && pPos.z < -20.0f);
	bool CombatToMovement = (pPos.x >= 11.0f && pPos.x <= 30.0f && pPos.z > -18.0f);
	bool CombatToFreerange = (pPos.x >= -30.0f && pPos.x <= -24.0f && pPos.z > -18.0f);
	bool FreerangeToCombat = (pPos.x >= -30.0f && pPos.x <= -24.0f && pPos.z < -20.0f);


	if (isPlayerCombatArea) {
		EnemyManager::GetIns().Update();
	}
	CollisionManager::GetIns().Update();
	ProjectileManager::GetIns().Update();
	ItemManager::GetIns().Update(&player);
	if (player.GetHP() <= 0) {
		player.revive();
	}
	
	switch (currentPhase) {
	case TutorialPhase::MOVEMENT:
		if (MovementToCombat) {
			currentPhase = TutorialPhase::COMBAT;
			EnemyManager::GetIns().Clear();
			currentEnemyInfo = -1;
		}
		break;
	case TutorialPhase::COMBAT: {
		static bool entered = false;
		if (isPlayerCombatArea) {
			entered = true;
		}
		if (entered && !isPlayerCombatArea) {
			EnemyManager::GetIns().Clear();
			entered = false;
		}
		if (CombatToMovement) {
			currentPhase = TutorialPhase::MOVEMENT;
			EnemyManager::GetIns().Clear();
			currentEnemyInfo = -1;
		}
		if (CombatToFreerange) {
			currentPhase = TutorialPhase::FREERANGE;
			EnemyManager::GetIns().Clear();
			currentEnemyInfo = -1;
		}

		//スポーンボタンの処理
		for (auto& btn : button) {
			if (btn.dummy && btn.dummy->GetHP() < btn.dummy->GetStatus().maxHP) {
				btn.dummy->revive();

				if (btn.spawnCT <= 0.0f) {

					EnemyManager::GetIns().Clear();

					currentEnemyInfo = btn.enemyType;
					VECTOR spawnPos = VGet(-18, 1, -24);

					if (btn.enemyType == 0)EnemyManager::GetIns().Spawn(std::make_unique<MeleeEnemy>(spawnPos, &player));
					if (btn.enemyType == 1)EnemyManager::GetIns().Spawn(std::make_unique<RifleEnemy>(spawnPos, &player));
					if (btn.enemyType == 2)EnemyManager::GetIns().Spawn(std::make_unique<SniperEnemy>(spawnPos, &player));
					if (btn.enemyType == 3)EnemyManager::GetIns().Spawn(std::make_unique<RollingEnemy>(spawnPos, &player));
					btn.spawnCT = 1.0f;
				}
			}
		}
		break;
	}
	case TutorialPhase::FREERANGE:
		if (FreerangeToCombat) {
			currentPhase = TutorialPhase::COMBAT;
			EnemyManager::GetIns().Clear();
			currentEnemyInfo = -1;
		}
		break;
	}
}

void TutorialScene::PauseUpdate() {
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
		if (pauseSelectNum == RESUME) isPaused = false;
		else if (pauseSelectNum == RETURN_TITLE) manager->ChangeScene(std::make_unique<TitleScene>(manager));
	}
}


void TutorialScene::Draw() {
	if (stageHandle != -1) MV1DrawModel(stageHandle);
	ItemManager::GetIns().SetCamPos(camera.GetPos());
	// ボタン（ダミー）の描画とテキスト
	for (auto& btn : button) {
		if (btn.dummy) {
			btn.dummy->Draw();	
			VECTOR camPos = camera.GetPos(); 
			float distance = VSize(VSub(camPos, btn.pos));
			if (distance <= 15.0f){
				VECTOR textPos3D = VAdd(btn.pos, VGet(0.0f, 2.0f, 0.0f));
				VECTOR sp = ConvWorldPosToScreenPos(textPos3D);
				if (sp.z >= 0 && sp.z <= 1.0f) {
					MV1_COLL_RESULT_POLY hit = MV1CollCheck_Line(stageHandle, -1, camPos, textPos3D);
					if (hit.HitFlag == 0) {
						const char* name = "";
						if (btn.enemyType == 0) name = "Melee";
						else if (btn.enemyType == 1) name = "Rifle";
						else if (btn.enemyType == 2) name = "Sniper";
						else if (btn.enemyType == 3) name = "Rolling";
						DrawFormatString((int)sp.x - 40, (int)sp.y, GetColor(0, 255, 255), "Shoot: %s", name);
					}
				}
			}
		}
	}

	for (auto& t : target) if (t) t->Draw();
	EnemyManager::GetIns().Draw();

	ProjectileManager::GetIns().Draw();
	ItemManager::GetIns().Draw();
	player.Draw();
	Debug::Draw();

	// --- HUD描画 ---
	const int colorTitle = GetColor(0, 255, 0);
	const int colorText = GetColor(0, 0, 0);
	const int colorWarning = GetColor(255, 100, 100);
	const int colorAlert = GetColor(255, 255, 0);
	const int colorInfo = GetColor(255, 200, 0);

	DrawString(20, 20, "- TUTORIAL -", colorTitle);

	switch (currentPhase) {
	case TutorialPhase::MOVEMENT:
		DrawString(20, 50, "[Movement Room]", colorTitle);
		DrawString(20, 80, "Move: [W][A][S][D] / L-Stick", colorText);
		DrawString(20, 100, "Sprint: [Shift] / PAD 2", colorText);
		DrawString(20, 120, "Jump: [Space] / PAD 5", colorText);
		DrawString(20, 140, "Slide: Sprint + [Ctrl] / PAD 6", colorText);
		DrawString(20, 180, ">> Proceed through the Gate", colorAlert);
		break;

	case TutorialPhase::COMBAT:
		DrawString(20, 50, "[Combat Room]", colorTitle);
		DrawString(20, 80, "Shoot the Target to spawn enemy.", colorText);
		DrawString(20, 200, ">> Proceed through the Gate", colorAlert);

		// スポーンした敵の情報
		if (currentEnemyInfo == 0) {
			DrawString(20, 110, "< Melee Enemy >", colorWarning);
			DrawString(20, 130, "Chases and attacks from close range.", colorText);
		}
		else if (currentEnemyInfo == 1) {
			DrawString(20, 110, "< Rifle Enemy >", colorWarning);
			DrawString(20, 130, "Shoots bullets from mid-range.", colorText);
		}
		else if (currentEnemyInfo == 2) {
			DrawString(20, 110, "< Sniper Enemy >", colorWarning);
			DrawString(20, 130, "High damage from long distance. Watch out for red laser.", colorText);
		}
		else if (currentEnemyInfo == 3) {
			DrawString(20, 110, "< Rolling Enemy >", colorWarning);
			DrawString(20, 130, "Fast movement. Head hitbox is inside its body.", colorText);
		}
		break;

	case TutorialPhase::FREERANGE:
		DrawString(20, 50, "[Free Range]", colorTitle);

		// 武器情報HUD（持っている武器に応じて変化）
		if (Weapon* w = player.GetWeapon()) {
			auto id = w->GetSpec().id;
			DrawString(20, 80, "Current Weapon Info:", colorInfo);

			switch (id) {
				case WeaponID::PIS:
				DrawString(20, 100, "PISTOL: Basic sidearm. Infinite ammo.", colorText); break;
				case WeaponID::AR:
				DrawString(20, 100, "ASSAULT RIFLE: Good fire rate and medium range.", colorText); break;
			case WeaponID::SR:
				DrawString(20, 100, "SNIPER RIFLE: High damage, slow fire rate. Good for headshots.", colorText); break;
			case WeaponID::SMG:
				DrawString(20, 100, "SMG: Very high fire rate, high recoil. Close range.", colorText); break;
			case WeaponID::LR:
				DrawString(20, 100, "ROCKET LAUNCHER: Area of effect damage. Deals splash damage.", colorText); break;
			}
		}
		DrawString(20, 150, "Pick up weapons from spawners and practice on targets.", colorText);
		DrawString(20, 180, "Press [ESC] / [START] to Pause and Return to Title.", colorText);
		break;
	}

	if (isPaused) PauseDraw();
}

void TutorialScene::PauseDraw() {
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
	DrawBox(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, GetColor(0, 0, 0), TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	DrawString(CENTER_X - 40, CENTER_Y - 100, "PAUSE", GetColor(0, 0, 0));
	int colorResume = (pauseSelectNum == RESUME) ? GetColor(255, 255, 0) : GetColor(0, 0, 0);
	int colorTitle = (pauseSelectNum == RETURN_TITLE) ? GetColor(255, 255, 0) : GetColor(0, 0, 0);
	DrawString(CENTER_X - 80, CENTER_Y + 40 * pauseSelectNum, ">", GetColor(0, 0, 0));
	DrawString(CENTER_X - 60, CENTER_Y, "Resume", colorResume);
	DrawString(CENTER_X - 60, CENTER_Y + 40, "Return Title", colorTitle);
}