#include "ResultScene.h"
#include "TitleScene.h"
#include "InputManager.h"
#include "EnemyManager.h"
#include "Parameter.h"

ResultScene::ResultScene(SceneManager* manager) : BaseScene(manager){}

ResultScene::~ResultScene() {}

void ResultScene::Init(){}

void ResultScene::Update() {
	if (InputManager::GetIns().IsActionTrigger(ActionID::MENU_SELECT)) {
		manager->ChangeScene(std::make_unique<TitleScene>(manager));
	}
}

void ResultScene::Draw() {
	DrawString(CENTER_X - 50, 100, "--- RESULT ---", GetColor(255, 255, 0));

	DrawFormatString(CENTER_X - 60, 150, GetColor(255, 255, 255), "SCORE:%d", manager->GetScore());

	DrawString(CENTER_X - 100, 200, "-Kill-", GetColor(200, 200, 200));
	int killMelee = EnemyManager::GetIns().GetKillCount(ENEMYTYPE::MELEE);
	int killRifle = EnemyManager::GetIns().GetKillCount(ENEMYTYPE::RIFLE);
	int killSniper = EnemyManager::GetIns().GetKillCount(ENEMYTYPE::SNIPER);
	int killRoll = EnemyManager::GetIns().GetKillCount(ENEMYTYPE::ROLLING);

	DrawFormatString(CENTER_X - 100, 230, GetColor(255, 255, 255), "Melee : %d", killMelee);
	DrawFormatString(CENTER_X - 100, 260, GetColor(255, 255, 255), "Rifle : %d", killRifle);
	DrawFormatString(CENTER_X - 100, 290, GetColor(255, 255, 255), "Sniper : %d", killSniper);
	DrawFormatString(CENTER_X - 100, 320, GetColor(255, 255, 255), "Roll : %d", killRoll);

	DrawString(CENTER_X + 80, 200, "-Accuracy-", GetColor(200, 200, 200));
	int shot = manager->GetShots();
	int hit = manager->GetHits();
	int head = manager->GetHeadShot();
	
	float accuracy = (shot > 0) ? ((float)hit / shot) * 100.0f : 0.0f;
	float hsAccuracy = (hit > 0) ? ((float)head / hit) * 100.0f : 0.0f;

	DrawFormatString(CENTER_X + 80, 230, GetColor(255, 255, 255), "Accuracy : %.1f%%", accuracy);
	DrawFormatString(CENTER_X + 80, 260, GetColor(255, 255, 255), "HS Accuracy : %.1f%%", hsAccuracy);


	const char*modeString = "";
	switch (manager->GetcurrentMode()) {
	case PlayMode::MODE_EASY:modeString = "EASY"; break;
	case PlayMode::MODE_NORMAL:modeString = "NORMAL"; break;
	case PlayMode::MODE_HARD:modeString = "HARD"; break;
	default: modeString = "UNKNOWN"; break;
		}
	DrawFormatString(CENTER_X - 60, 380, GetColor(255, 200, 0), "- RANKING(%s) -", modeString);
	const auto& ranking = manager->GetRanking();
	bool highlight = false;
	for (size_t i = 0; i < ranking.size(); ++i) {
		int color = GetColor(255, 255, 255);
		if (!highlight && ranking[i] == manager->GetScore()) {
			color = GetColor(255, 100, 100);
			highlight = true;
		}
		DrawFormatString(CENTER_X - 60, 410 + (int)i * 30, color, "%d.%d", i + 1, ranking[i]);
	}

	DrawString(CENTER_X - 410, WINDOW_HEIGHT - 100, "PRESS[F/A] to Return", GetColor(255, 255, 255));
}