#include "ResultScene.h"
#include "TitleScene.h"
#include "InputManager.h"
#include "EnemyManager.h"
#include "Parameter.h"
#include "ResourceManager.h"
#include "TextManager.h"

ResultScene::ResultScene(SceneManager* manager,int bghandle)
	: BaseScene(manager)
	, bgHandle(bghandle){}

ResultScene::~ResultScene() {
	if (bgHandle != -1) {
		DeleteGraph(bgHandle);
	}
}

void ResultScene::Init(){
	fontLarge = ResourceManager::GetIns().GetFont("Century Gothic", 60, 2);
	fontMedium = ResourceManager::GetIns().GetFont("メイリオ", 30, 1);
	fontSmall = ResourceManager::GetIns().GetFont("メイリオ", 22, 1);
}

void ResultScene::Update() {
	if (InputManager::GetIns().IsActionTrigger(ActionID::MENU_SELECT)) {
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

	// 1. タイトル位置に見出し
	::DrawStringToHandle(TITLE_X, TITLE_Y, "Result", white, fontLarge);

	// 2. 一番上にスコア（中央にデカく表示）
	::DrawFormatStringToHandle(CENTER_X - 150, SCORE_Y, yellow, fontLarge, "Score : %d", result.currentScore);


	// 3. 左上：死因
	::DrawStringToHandle(COL_LEFT_X, ROW_TOP_Y, "- 死因 -", gray, fontMedium);
	const char* causeStr = TextManager::GetIns().GetCauseName(result.causeOfDeath);
	::DrawFormatStringToHandle(COL_LEFT_X + 40, ROW_TOP_Y + LINE_HEIGHT, red, fontMedium, "%s", causeStr);


	// 4. 右上：射撃精度
	::DrawStringToHandle(COL_RIGHT_X, ROW_TOP_Y, "- 射撃精度 -", gray, fontMedium);
	int shot = result.Shot;
	int hit = result.totalHit;
	int head = result.totalHeadHit;
	float accuracy = (shot > 0) ? ((float)hit / shot) * 100.0f : 0.0f;
	float hsAccuracy = (hit > 0) ? ((float)head / hit) * 100.0f : 0.0f;

	::DrawFormatStringToHandle(COL_RIGHT_X + 40, ROW_TOP_Y + LINE_HEIGHT, white, fontMedium, "ヒット率 : %.1f%%", accuracy);
	::DrawFormatStringToHandle(COL_RIGHT_X + 40, ROW_TOP_Y + LINE_HEIGHT * 2, white, fontMedium, "ヘッドショット率 : %.1f%%", hsAccuracy);


	// 5. 左下：キル数
	::DrawStringToHandle(COL_LEFT_X, ROW_BOTTOM_Y, "- キル数 -", gray, fontMedium);
	int killMelee = EnemyManager::GetIns().GetKillCount(ENEMYTYPE::MELEE);
	int killRifle = EnemyManager::GetIns().GetKillCount(ENEMYTYPE::RIFLE);
	int killSniper = EnemyManager::GetIns().GetKillCount(ENEMYTYPE::SNIPER);
	int killRoll = EnemyManager::GetIns().GetKillCount(ENEMYTYPE::ROLLING);

	::DrawFormatStringToHandle(COL_LEFT_X + 40, ROW_BOTTOM_Y + LINE_HEIGHT, white, fontMedium, "近接 : %d", killMelee);
	::DrawFormatStringToHandle(COL_LEFT_X + 40, ROW_BOTTOM_Y + LINE_HEIGHT * 2, white, fontMedium, "ライフル : %d", killRifle);
	::DrawFormatStringToHandle(COL_LEFT_X + 40, ROW_BOTTOM_Y + LINE_HEIGHT * 3, white, fontMedium, "スナイパー : %d", killSniper);
	::DrawFormatStringToHandle(COL_LEFT_X + 40, ROW_BOTTOM_Y + LINE_HEIGHT * 4, white, fontMedium, "爆弾 : %d", killRoll);


	// 6. 右下：ランキング
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
		// 今回のスコアがランクインしていたら赤色（目立つ色）でハイライト
		if (!highlight && ranking[i] == result.currentScore) {
			color = red;
			highlight = true;
		}
		::DrawFormatStringToHandle(COL_RIGHT_X + 40, ROW_BOTTOM_Y + LINE_HEIGHT + (int)i * LINE_HEIGHT, color, fontMedium, "%d.   %d", i + 1, ranking[i]);
	}


	// 7. 下部黒帯と操作説明（TitleSceneと完全に同じ座標・バランス）
	::DrawBox(0, WINDOW_HEIGHT - 50, WINDOW_WIDTH, WINDOW_HEIGHT, GetColor(0, 0, 0), TRUE);
	::DrawStringToHandle(WINDOW_WIDTH - 600, WINDOW_HEIGHT - 30, "F : タイトルに戻る", white, fontSmall);
}