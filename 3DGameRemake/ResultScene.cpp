#include "ResultScene.h"
#include "TitleScene.h"
#include "InputManager.h"
#include "EnemyManager.h"
#include "ResourceManager.h"
#include "TextManager.h"
#include "SoundManager.h"
#include "Param/Global.h"
#include "Param/Scene.h"
#include "Param/System.h"

ResultScene::ResultScene(SceneManager* manager, int bghandle)
	: BaseScene(manager)
	, bgHandle(bghandle) {}

ResultScene::~ResultScene() {
	if (bgHandle != -1) {
		DeleteGraph(bgHandle);
	}
}

void ResultScene::Init() {
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
	const int white = GetColor(Global::Palette::WHITE.r, Global::Palette::WHITE.g, Global::Palette::WHITE.b);
	const int yellow = GetColor(Global::Palette::YELLOW.r, Global::Palette::YELLOW.g, Global::Palette::YELLOW.b);
	const int gray = GetColor(Global::Palette::GRAY.r, Global::Palette::GRAY.g, Global::Palette::GRAY.b);
	const int red = GetColor(Global::Palette::RED_LIGHT.r, Global::Palette::RED_LIGHT.g, Global::Palette::RED_LIGHT.b);

	if (bgHandle != -1) {
		SetDrawBright(Scene::Result::RESULT_BG_BRIGHTNESS, Scene::Result::RESULT_BG_BRIGHTNESS, Scene::Result::RESULT_BG_BRIGHTNESS);
		DrawGraph(0, 0, bgHandle, FALSE);
		SetDrawBright(255, 255, 255);
	}

	const GameResult& result = manager->GetResult();

	::DrawStringToHandle(Scene::Result::RESULT_TITLE_X, Scene::Result::RESULT_TITLE_Y, "Result", white, fontLarge);

	::DrawFormatStringToHandle(System::Window::CENTER_X + Scene::Result::RESULT_SCORE_OFFSET_X, Scene::Result::RESULT_SCORE_Y, yellow, fontLarge, "Score : %d", result.currentScore);


	::DrawStringToHandle(Scene::Result::RESULT_COL_LEFT_X, Scene::Result::RESULT_ROW_TOP_Y, "- 死因 -", gray, fontMedium);
	const char* causeStr = TextManager::GetIns().GetCauseName(result.causeOfDeath);
	::DrawFormatStringToHandle(Scene::Result::RESULT_COL_LEFT_X + Scene::Result::RESULT_ITEM_INDENT_X, Scene::Result::RESULT_ROW_TOP_Y + Scene::Result::RESULT_LINE_HEIGHT, red, fontMedium, "%s", causeStr);


	::DrawStringToHandle(Scene::Result::RESULT_COL_RIGHT_X, Scene::Result::RESULT_ROW_TOP_Y, "- 射撃精度 -", gray, fontMedium);
	int shot = result.Shot;
	int hit = result.totalHit;
	int head = result.totalHeadHit;
	float accuracy = (shot > 0) ? ((float)hit / shot) * Scene::Result::PERCENTAGE_MULTIPLIER : 0.0f;
	float hsAccuracy = (hit > 0) ? ((float)head / hit) * Scene::Result::PERCENTAGE_MULTIPLIER : 0.0f;

	::DrawFormatStringToHandle(Scene::Result::RESULT_COL_RIGHT_X + Scene::Result::RESULT_ITEM_INDENT_X, Scene::Result::RESULT_ROW_TOP_Y + Scene::Result::RESULT_LINE_HEIGHT, white, fontMedium, "ヒット率 : %.1f%%", accuracy);
	::DrawFormatStringToHandle(Scene::Result::RESULT_COL_RIGHT_X + Scene::Result::RESULT_ITEM_INDENT_X, Scene::Result::RESULT_ROW_TOP_Y + Scene::Result::RESULT_LINE_HEIGHT * 2, white, fontMedium, "ヘッドショット率 : %.1f%%", hsAccuracy);


	::DrawStringToHandle(Scene::Result::RESULT_COL_LEFT_X, Scene::Result::RESULT_ROW_BOTTOM_Y, "- キル数 -", gray, fontMedium);
	int killMelee = EnemyManager::GetIns().GetKillCount(ENEMYTYPE::MELEE);
	int killRifle = EnemyManager::GetIns().GetKillCount(ENEMYTYPE::RIFLE);
	int killSniper = EnemyManager::GetIns().GetKillCount(ENEMYTYPE::SNIPER);
	int killRoll = EnemyManager::GetIns().GetKillCount(ENEMYTYPE::ROLLING);

	::DrawFormatStringToHandle(Scene::Result::RESULT_COL_LEFT_X + Scene::Result::RESULT_ITEM_INDENT_X, Scene::Result::RESULT_ROW_BOTTOM_Y + Scene::Result::RESULT_LINE_HEIGHT, white, fontMedium, "近接 : %d", killMelee);
	::DrawFormatStringToHandle(Scene::Result::RESULT_COL_LEFT_X + Scene::Result::RESULT_ITEM_INDENT_X, Scene::Result::RESULT_ROW_BOTTOM_Y + Scene::Result::RESULT_LINE_HEIGHT * 2, white, fontMedium, "ライフル : %d", killRifle);
	::DrawFormatStringToHandle(Scene::Result::RESULT_COL_LEFT_X + Scene::Result::RESULT_ITEM_INDENT_X, Scene::Result::RESULT_ROW_BOTTOM_Y + Scene::Result::RESULT_LINE_HEIGHT * 3, white, fontMedium, "スナイパー : %d", killSniper);
	::DrawFormatStringToHandle(Scene::Result::RESULT_COL_LEFT_X + Scene::Result::RESULT_ITEM_INDENT_X, Scene::Result::RESULT_ROW_BOTTOM_Y + Scene::Result::RESULT_LINE_HEIGHT * 4, white, fontMedium, "爆弾 : %d", killRoll);


	const char* modeString = "";
	switch (manager->GetcurrentMode()) {
	case PlayMode::MODE_EASY:modeString = "EASY"; break;
	case PlayMode::MODE_NORMAL:modeString = "NORMAL"; break;
	case PlayMode::MODE_HARD:modeString = "HARD"; break;
	default: modeString = "UNKNOWN"; break;
	}

	::DrawFormatStringToHandle(Scene::Result::RESULT_COL_RIGHT_X, Scene::Result::RESULT_ROW_BOTTOM_Y, yellow, fontMedium, "- ランキング (%s) -", modeString);

	const auto& ranking = manager->GetRanking();
	bool highlight = false;
	for (size_t i = 0; i < ranking.size(); ++i) {
		int color = white;
		if (!highlight && ranking[i] == result.currentScore) {
			color = red;
			highlight = true;
		}
		::DrawFormatStringToHandle(Scene::Result::RESULT_COL_RIGHT_X + Scene::Result::RESULT_ITEM_INDENT_X, Scene::Result::RESULT_ROW_BOTTOM_Y + Scene::Result::RESULT_LINE_HEIGHT + (int)i * Scene::Result::RESULT_LINE_HEIGHT, color, fontMedium, "%d.   %d", i + 1, ranking[i]);
	}


	::DrawBox(0, System::Window::WINDOW_HEIGHT - Scene::Result::RESULT_HELP_BAR_HEIGHT, System::Window::WINDOW_WIDTH, System::Window::WINDOW_HEIGHT, GetColor(Global::Palette::BLACK.r, Global::Palette::BLACK.g, Global::Palette::BLACK.b), TRUE);

	std::string menuSel = TextManager::GetIns().GetActionKeyString(ActionID::MENU_SELECT);
	std::string helpText = menuSel + " : タイトルに戻る";

	int textWidth = GetDrawStringWidthToHandle(helpText.c_str(), static_cast<int>(helpText.length()), fontSmall);
	::DrawStringToHandle(System::Window::WINDOW_WIDTH - textWidth - Scene::Common::HELP_TEXT_MARGIN_X, System::Window::WINDOW_HEIGHT - Scene::Common::HELP_TEXT_MARGIN_Y, helpText.c_str(), white, fontSmall);
}