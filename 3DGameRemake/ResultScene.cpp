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
	const int white = GetColor(255, 255, 255);
	const int yellow = GetColor(255, 255, 0);
	const int gray = GetColor(200, 200, 200);
	const int red = GetColor(255, 100, 100);
	
	const int BASE_Y = 100;
	const int LINE_HEIGHT = 30;
	const GameResult& result = manager->GetResult();
	DrawString(CENTER_X - 50, BASE_Y, "--- RESULT ---", yellow);

	//スコア
	DrawFormatString(CENTER_X - 60, BASE_Y +LINE_HEIGHT * 2, white, "SCORE:%d", result.currentScore);

	//キル数
	int killLineX = CENTER_X - 100;
	int killLineY = BASE_Y + 100;

	DrawString(killLineX,killLineY , "-Kill-", gray);
	int killMelee = EnemyManager::GetIns().GetKillCount(ENEMYTYPE::MELEE);
	int killRifle = EnemyManager::GetIns().GetKillCount(ENEMYTYPE::RIFLE);
	int killSniper = EnemyManager::GetIns().GetKillCount(ENEMYTYPE::SNIPER);
	int killRoll = EnemyManager::GetIns().GetKillCount(ENEMYTYPE::ROLLING);

	DrawFormatString(killLineX, killLineY + LINE_HEIGHT    , white, "Melee : %d", killMelee);
	DrawFormatString(killLineX, killLineY + LINE_HEIGHT * 2, white, "Rifle : %d", killRifle);
	DrawFormatString(killLineX, killLineY + LINE_HEIGHT * 3, white, "Sniper : %d", killSniper);
	DrawFormatString(killLineX, killLineY + LINE_HEIGHT * 4, white, "Roll : %d", killRoll);


	//精度
	int accX = CENTER_X + 80;
	int accY = BASE_Y + 100;

	DrawString(accX, accY, "-Accuracy-", gray);

	int shot = result.Shot;
	int hit = result.totalHit;
	int head = result.totalHeadHit;
	float accuracy = (shot > 0) ? ((float)hit / shot) * 100.0f : 0.0f;
	float hsAccuracy = (hit > 0) ? ((float)head / hit) * 100.0f : 0.0f;

	DrawFormatString(accX, accY + LINE_HEIGHT, white, "Accuracy : %.1f%%", accuracy);
	DrawFormatString(accX, accY + LINE_HEIGHT * 2, white, "HS Accuracy : %.1f%%", hsAccuracy);


	//ランキング
	int rankY = BASE_Y + LINE_HEIGHT * 10;

	const char*modeString = "";
	switch (manager->GetcurrentMode()) {
	case PlayMode::MODE_EASY:modeString = "EASY"; break;
	case PlayMode::MODE_NORMAL:modeString = "NORMAL"; break;
	case PlayMode::MODE_HARD:modeString = "HARD"; break;
	default: modeString = "UNKNOWN"; break;
	}

	DrawFormatString(CENTER_X - 60, rankY, GetColor(255, 200, 0), "- RANKING(%s) -", modeString);

	const auto& ranking = manager->GetRanking();
	bool highlight = false;
	for (size_t i = 0; i < ranking.size(); ++i) {
		int color = white;
		if (!highlight && ranking[i] == result.currentScore) {
			color = red;
			highlight = true;
		}
		DrawFormatString(CENTER_X - 60, rankY + LINE_HEIGHT + (int)i * LINE_HEIGHT, color, "%d.%d", i + 1, ranking[i]);
	}

	DrawString(CENTER_X - 410, WINDOW_HEIGHT - 100, "PRESS[F/A] to Return", white);
}