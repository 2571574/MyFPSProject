#include "ResultScene.h"
#include "TitleScene.h"
#include "InputManager.h"
#include "EnemyManager.h"
#include "Parameter.h"
#include "ResourceManager.h"
#include "TextManager.h"
#include "SoundManager.h"

ResultScene::ResultScene(SceneManager* manager,int bghandle)
	: BaseScene(manager)
	, bgHandle(bghandle){}

ResultScene::~ResultScene() {
	if (bgHandle != -1) {
		DeleteGraph(bgHandle);
	}
}

void ResultScene::Init(){
	fontLarge = ResourceManager::GetIns().GetFont("Resource/Font/JetBrainsMono_60.dft");
	fontMedium = ResourceManager::GetIns().GetFont("Resource/Font/NotoSansJP_30.dft");
	fontSmall = ResourceManager::GetIns().GetFont("Resource/Font/NotoSansJP_22.dft");
}

void ResultScene::Update() {
	if (InputManager::GetIns().IsActionTrigger(ActionID::MENU_SELECT)) {
		SoundManager::GetIns().PlaySE("Resource/Sound/select.ogg");
		manager->ChangeScene(std::make_unique<TitleScene>(manager));
	}
}

void ResultScene::Draw() {
	const int white = GetColor(255, 255, 255);
	const int yellow = GetColor(255, 255, 0);
	const int gray = GetColor(180, 180, 180);
	const int red = GetColor(255, 100, 100);

	if (bgHandle != -1) {
		SetDrawBright(120, 120, 120);
		DrawGraph(0, 0, bgHandle, FALSE);
		SetDrawBright(255, 255, 255);
	}

	// ==========================================
	// レイアウト調整用の定数（ここで全体のバランスをいじれます）
	const int TITLE_X = 150;         // 「Result」見出しのX座標
	const int TITLE_Y = 100;          // 「Result」見出しのY座標

	const int SCORE_Y = 200;         // 一番上のスコアのY座標

	const int COL_LEFT_X = 300;      // 左列（死因・キル数）の基準X座標
	const int COL_RIGHT_X = 1100;    // 右列（射撃精度・ランキング）の基準X座標

	const int ROW_TOP_Y = 300;       // 上段（死因・射撃精度）の開始Y座標
	const int ROW_BOTTOM_Y = 550;    // 下段（キル数・ランキング）の開始Y座標
	const int LINE_HEIGHT = 45;      // 各項目の行間
	// ==========================================

	const GameResult& result = manager->GetResult();

	::DrawStringToHandle(TITLE_X, TITLE_Y, "Result", white, fontLarge);

	::DrawFormatStringToHandle(CENTER_X - 150, SCORE_Y, yellow, fontLarge, "Score : %d", result.currentScore);


	::DrawStringToHandle(COL_LEFT_X, ROW_TOP_Y, "- 死因 -", gray, fontMedium);
	const char* causeStr = TextManager::GetIns().GetCauseName(result.causeOfDeath);
	::DrawFormatStringToHandle(COL_LEFT_X + 40, ROW_TOP_Y + LINE_HEIGHT, red, fontMedium, "%s", causeStr);


	::DrawStringToHandle(COL_RIGHT_X, ROW_TOP_Y, "- 射撃精度 -", gray, fontMedium);
	int shot = result.Shot;
	int hit = result.totalHit;
	int head = result.totalHeadHit;
	float accuracy = (shot > 0) ? ((float)hit / shot) * 100.0f : 0.0f;
	float hsAccuracy = (hit > 0) ? ((float)head / hit) * 100.0f : 0.0f;

	::DrawFormatStringToHandle(COL_RIGHT_X + 40, ROW_TOP_Y + LINE_HEIGHT, white, fontMedium, "ヒット率 : %.1f%%", accuracy);
	::DrawFormatStringToHandle(COL_RIGHT_X + 40, ROW_TOP_Y + LINE_HEIGHT * 2, white, fontMedium, "ヘッドショット率 : %.1f%%", hsAccuracy);


	::DrawStringToHandle(COL_LEFT_X, ROW_BOTTOM_Y, "- キル数 -", gray, fontMedium);
	int killMelee = EnemyManager::GetIns().GetKillCount(ENEMYTYPE::MELEE);
	int killRifle = EnemyManager::GetIns().GetKillCount(ENEMYTYPE::RIFLE);
	int killSniper = EnemyManager::GetIns().GetKillCount(ENEMYTYPE::SNIPER);
	int killRoll = EnemyManager::GetIns().GetKillCount(ENEMYTYPE::ROLLING);

	::DrawFormatStringToHandle(COL_LEFT_X + 40, ROW_BOTTOM_Y + LINE_HEIGHT, white, fontMedium, "近接 : %d", killMelee);
	::DrawFormatStringToHandle(COL_LEFT_X + 40, ROW_BOTTOM_Y + LINE_HEIGHT * 2, white, fontMedium, "ライフル : %d", killRifle);
	::DrawFormatStringToHandle(COL_LEFT_X + 40, ROW_BOTTOM_Y + LINE_HEIGHT * 3, white, fontMedium, "スナイパー : %d", killSniper);
	::DrawFormatStringToHandle(COL_LEFT_X + 40, ROW_BOTTOM_Y + LINE_HEIGHT * 4, white, fontMedium, "爆弾 : %d", killRoll);


	const char* modeString = "";
	switch (manager->GetcurrentMode()) {
	case PlayMode::MODE_EASY:modeString = "EASY"; break;
	case PlayMode::MODE_NORMAL:modeString = "NORMAL"; break;
	case PlayMode::MODE_HARD:modeString = "HARD"; break;
	default: modeString = "UNKNOWN"; break;
	}

	::DrawFormatStringToHandle(COL_RIGHT_X, ROW_BOTTOM_Y, yellow, fontMedium, "- ランキング (%s) -", modeString);

	const auto& ranking = manager->GetRanking();
	bool highlight = false;
	for (size_t i = 0; i < ranking.size(); ++i) {
		int color = white;
		if (!highlight && ranking[i] == result.currentScore) {
			color = red;
			highlight = true;
		}
		::DrawFormatStringToHandle(COL_RIGHT_X + 40, ROW_BOTTOM_Y + LINE_HEIGHT + (int)i * LINE_HEIGHT, color, fontMedium, "%d.   %d", i + 1, ranking[i]);
	}


	::DrawBox(0, WINDOW_HEIGHT - 50, WINDOW_WIDTH, WINDOW_HEIGHT, GetColor(0, 0, 0), TRUE);

	std::string menuSel = TextManager::GetIns().GetActionKeyString(ActionID::MENU_SELECT);
	std::string helpText = menuSel + " : タイトルに戻る";

	int textWidth = GetDrawStringWidthToHandle(helpText.c_str(), static_cast<int>(helpText.length()), fontSmall);
	::DrawStringToHandle(WINDOW_WIDTH - textWidth - 50, WINDOW_HEIGHT - 30, helpText.c_str(), white, fontSmall);
}