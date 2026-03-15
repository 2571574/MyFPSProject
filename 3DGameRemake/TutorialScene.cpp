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
TutorialScene::TutorialScene(SceneManager* manager)
	:BaseScene(manager)
	, player(VGet(20.0f, 0.0f, 28.0f), &camera, PlayMode::MODE_TUTORIAL)
	, stageHandle(-1)
	, isPaused(false)
	, pauseSelectNum(0) {
}

TutorialScene::~TutorialScene() {
	for (auto& btn : button) {
		if (btn.dummy)CollisionManager::GetIns().Unregister(btn.dummy.get());
	}
	for (auto& t : target) {
		if (t)CollisionManager::GetIns().Unregister(t.get());
	}
	if (currentEnemy)CollisionManager::GetIns().Unregister(currentEnemy.get());
	EnemyManager::GetIns().Clear();
	ProjectileManager::GetIns().Clear();
	ItemManager::GetIns().Clear();
} 

void TutorialScene::Init() {
	currentPhase = TutorialPhase::MOVEMENT;
	currentEnemyInfo = -1;

	stageHandle = MV1LoadModel("Resource/TutorialArena.mv1");

	MV1SetPosition(stageHandle, VGet(0.0f, 0.0f, 0.0f));
	MV1SetScale(stageHandle, VGet(0.02f, 0.02f, 0.02f));
	MV1SetupCollInfo(stageHandle, -1, 8, 8, 8);
	player.SetStageHandle(stageHandle);
	CollisionManager::GetIns().SetStageHandle(stageHandle);

	for (int i = 0; i < 4; i++) {
		SpawnButton btn;
		btn.pos = VGet(-6.0f + i * 4.0f, 0.0f, 25.0f);
		btn.dummy = std::make_unique<Dummy>(btn.pos,&player);
		btn.enemyType = i;
		CollisionManager::GetIns().Register(btn.dummy.get());
		button.push_back(std::move(btn));
	}

	std::vector<VECTOR>spawnerPos = {
		VGet(10.0f,0.4f,45.0f),VGet(5.0f,0.4f,45.0f),VGet(-5.0f,0.4f,45.0f),VGet(-10.0f,0.4f,45.0f) };
	ItemManager::GetIns().InitSpawners(spawnerPos);

	for (int i = 0; i < 3; ++i) {
		auto Target = std::make_unique<Dummy>(VGet(-5.0f + i * 5.0f, 0.0f, 50.0f + i * 10.0f),&player);
		CollisionManager::GetIns().Register(Target.get());
		target.push_back(std::move(Target));
	}
}

void TutorialScene::Update() {
	Time::GetIns().Update();
	Debug::Update();

	if (InputManager::GetIns().IsActionTrigger(ActionID::PAUSE)) {
		isPaused = !isPaused;
		pauseSelectNum = 0;
	}
	if (isPaused) {
		PauseUpdate();
		return;
	}
	player.Update();
	for (auto& btn : button) {
		if (btn.dummy)btn.dummy->Update();
	}
	for (auto& t : target) {
		if (t) {
			t->Update();
			if (t->GetHP() <= 0) t->revive();
		}
	}

	if (currentEnemy) {
		currentEnemy->Update();
		if (!currentEnemy->IsAlive()) {
			CollisionManager::GetIns().Unregister(currentEnemy.get());
			currentEnemy.reset();
		}
	}

	CollisionManager::GetIns().Update(&player, &EnemyManager::GetIns());
	ProjectileManager::GetIns().Update();
	ItemManager::GetIns().Update(&player);

	if (player.GetHP() <= 0) {
		player.revive();
	}

	VECTOR pPos = player.GetPos();
	auto IsInZone = [](VECTOR pos, float minX, float maxX, float minZ, float maxZ) {
		return (pos.x >= minX && pos.x <= maxX && pos.z >= minZ && pos.z <= maxZ);
	};
	switch (currentPhase) {
	case TutorialPhase::MOVEMENT:
		if (IsInZone(pPos,11.5f,30.0f,-18.0f,-20.0f)) {
			currentPhase = TutorialPhase::COMBAT;
		}
		break;
	case TutorialPhase::COMBAT: 
		if (IsInZone(pPos, 11.5f, 30.0f, -18.0f, -20.0f)) {
			currentPhase = TutorialPhase::FREERANGE;
		}

		for (auto& btn : button) {
			if (btn.dummy && btn.dummy->GetHP() < btn.dummy->GetStatus().maxHP) {
				btn.dummy->revive();

				if (currentEnemy) {
					CollisionManager::GetIns().Unregister(currentEnemy.get());
					currentEnemy.reset();
				}

				currentEnemyInfo = btn.enemyType;
				VECTOR spawnPos = VGet(0, 0, 35.0f);

				if (btn.enemyType == 0)currentEnemy = std::make_unique<MeleeEnemy>(spawnPos, &player);
				if (btn.enemyType == 1)currentEnemy = std::make_unique<RifleEnemy>(spawnPos, &player);
				if (btn.enemyType == 2)currentEnemy = std::make_unique<SniperEnemy>(spawnPos, &player);
				if (btn.enemyType == 3)currentEnemy = std::make_unique<RollingEnemy>(spawnPos, &player); 

				if (currentEnemy)CollisionManager::GetIns().Register(currentEnemy.get());
			}
		}
		break;
	case TutorialPhase::FREERANGE:
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

	// ボタン（ダミー）の描画とテキスト
	for (auto& btn : button) {
		if (btn.dummy) {
			btn.dummy->Draw();
			VECTOR sp = ConvWorldPosToScreenPos(VAdd(btn.pos, VGet(0, 2.0f, 0)));
			if (sp.z >= 0 && sp.z <= 1.0f) {
				const char* name = "";
				if (btn.enemyType == 0) name = "Melee";
				else if (btn.enemyType == 1) name = "Rifle";
				else if (btn.enemyType == 2) name = "Sniper";
				else if (btn.enemyType == 3) name = "Rolling";
				DrawFormatString((int)sp.x - 40, (int)sp.y, GetColor(0, 255, 255), "Shoot: %s", name);
			}
		}
	}

	for (auto& t : target) if (t) t->Draw();
	if (currentEnemy) currentEnemy->Draw();

	ProjectileManager::GetIns().Draw();
	player.Draw();
	ItemManager::GetIns().Draw();
	Debug::Draw();

	// --- HUD描画 ---
	int textColor = GetColor(0, 255, 0);
	DrawString(20, 20, "- TUTORIAL -", textColor);

	switch (currentPhase) {
	case TutorialPhase::MOVEMENT:
		DrawString(20, 50, "[Movement Room]", textColor);
		DrawString(20, 80, "Move: [W][A][S][D] / L-Stick", GetColor(0, 0, 0));
		DrawString(20, 100, "Sprint: [Shift] / PAD 2", GetColor(0, 0, 0));
		DrawString(20, 120, "Jump: [Space] / PAD 5", GetColor(0, 0, 0));
		DrawString(20, 140, "Slide: Sprint + [Ctrl] / PAD 6", GetColor(0, 0, 0));
		DrawString(20, 180, ">> Proceed through the Gate", GetColor(255, 255, 0));
		break;

	case TutorialPhase::COMBAT:
		DrawString(20, 50, "[Combat Room]", textColor);
		DrawString(20, 80, "Shoot the Target to spawn enemy.", GetColor(0, 0, 0));
		DrawString(20, 200, ">> Proceed through the Gate", GetColor(255, 255, 0));

		// スポーンした敵の情報
		if (currentEnemyInfo == 0) {
			DrawString(20, 110, "< Melee Enemy >", GetColor(255, 100, 100));
			DrawString(20, 130, "Chases and attacks from close range.", GetColor(0, 0, 0));
		}
		else if (currentEnemyInfo == 1) {
			DrawString(20, 110, "< Rifle Enemy >", GetColor(255, 100, 100));
			DrawString(20, 130, "Shoots bullets from mid-range.", GetColor(0, 0, 0));
		}
		else if (currentEnemyInfo == 2) {
			DrawString(20, 110, "< Sniper Enemy >", GetColor(255, 100, 100));
			DrawString(20, 130, "High damage from long distance. Watch out for red laser.", GetColor(0, 0, 0));
		}
		else if (currentEnemyInfo == 3) {
			DrawString(20, 110, "< Rolling Enemy >", GetColor(255, 100, 100));
			DrawString(20, 130, "Fast movement. Head hitbox is inside its body.", GetColor(0, 0, 0));
		}
		break;

	case TutorialPhase::FREERANGE:
		DrawString(20, 50, "[Free Range]", textColor);

		// 武器情報HUD（持っている武器に応じて変化）
		if (Weapon* w = player.GetWeapon()) {
			auto id = w->GetSpec().id;
			DrawString(20, 80, "Current Weapon Info:", GetColor(255, 200, 0));

			switch (id) {
				case WeaponID::PIS:
				DrawString(20, 100, "PISTOL: Basic sidearm. Infinite ammo.", GetColor(0, 0, 0)); break;
				case WeaponID::AR:
				DrawString(20, 100, "ASSAULT RIFLE: Good fire rate and medium range.", GetColor(0, 0, 0)); break;
			case WeaponID::SR:
				DrawString(20, 100, "SNIPER RIFLE: High damage, slow fire rate. Good for headshots.", GetColor(0, 0, 0)); break;
			case WeaponID::SMG:
				DrawString(20, 100, "SMG: Very high fire rate, high recoil. Close range.", GetColor(0, 0, 0)); break;
			case WeaponID::LR:
				DrawString(20, 100, "ROCKET LAUNCHER: Area of effect damage. Deals splash damage.", GetColor(0, 0, 0)); break;
			}
		}
		DrawString(20, 150, "Pick up weapons from spawners and practice on targets.", GetColor(0, 0, 0));
		DrawString(20, 180, "Press [ESC] / [START] to Pause and Return to Title.", GetColor(0, 0, 0));
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