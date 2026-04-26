#include "SceneManager.h"
#include "GameScene.h"
#include "TitleScene.h"
#include "SoundManager.h"
#include "Param/Global.h"
#include "Param/Scene.h"
#include "Param/System.h"

#include <fstream>
#include <algorithm>
#include <functional>

SceneManager::SceneManager()
	: exitTag(false)
	, currentMode(PlayMode::MODE_NORMAL) {
	ranking.resize((int)PlayMode::MODE_MAX, std::vector<int>(Scene::Manager::MAX_RECORD, 0));
	LoadRanking();
	ChangeScene(std::make_unique<TitleScene>(this));
}

void SceneManager::ChangeScene(std::unique_ptr<BaseScene> nextscene) {
	if (fadeState != FadeState::NONE)return;

	if (currentScene == nullptr) {
		currentScene = std::move(nextscene);
		currentScene->Init();
		fadeState = FadeState::FADEIN;
		fadeAlpha = 1.0f;
		return;
	}
	nextScenePending = std::move(nextscene);
	fadeState = FadeState::FADEOUT;
	fadeAlpha = 0.0f;
}

void SceneManager::Update() {
	float dt = Time::GetIns().GetDelta();
	//フェードアウト
	if (fadeState == FadeState::FADEOUT) {
		fadeAlpha += Scene::Manager::FADE_SPEED * dt;
		if (fadeAlpha >= 1.0f) {
			fadeAlpha = 1.0f;

			SoundManager::GetIns().StopAll();

			currentScene = std::move(nextScenePending);
			currentScene->Init();
			fadeState = FadeState::FADEIN;
		}
	}
	//フェードイン
	else if (fadeState == FadeState::FADEIN) {
		fadeAlpha -= Scene::Manager::FADE_SPEED * dt;
		if (fadeAlpha <= 0.0f) {
			fadeAlpha = 0.0f;
			fadeState = FadeState::NONE;
		}
	}
	if (currentScene) {
		currentScene->Update();
	}
}

void SceneManager::Draw() {
	if (currentScene) {
		currentScene->Draw();
	}

	if (fadeState != FadeState::NONE) {
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(fadeAlpha * Scene::Manager::ALPHA_MAX_SCALE));
		DrawBox(0, 0, System::Window::WINDOW_WIDTH, System::Window::WINDOW_HEIGHT, GetColor(Global::Palette::BLACK.r, Global::Palette::BLACK.g, Global::Palette::BLACK.b), TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
}

void SceneManager::SetScore(int score) {
	lastResult.currentScore = score;

	if (currentMode == PlayMode::MODE_TUTORIAL) {
		return;
	}

	//ランキングを更新
	int modeIndex = (int)currentMode;
	ranking[modeIndex].push_back(score);
	std::sort(ranking[modeIndex].begin(), ranking[modeIndex].end(), std::greater<int>());
	if (ranking[modeIndex].size() > Scene::Manager::MAX_RECORD) {
		ranking[modeIndex].resize(Scene::Manager::MAX_RECORD);
	}
	SaveRanking();
}


void SceneManager::SaveRanking() {
	std::ofstream ofs(RANKING_FILE, std::ios::binary | std::ios::trunc);
	if (ofs.is_open()) {
		for (int m = 0; m < (int)PlayMode::MODE_MAX; ++m) {
			for (int score : ranking[m]) {
				ofs.write(reinterpret_cast<const char*>(&score), sizeof(int));
			}
		}
		ofs.close();
	}
}


void SceneManager::LoadRanking() {
	std::ifstream ifs(RANKING_FILE, std::ios::binary);
	if (ifs.is_open()) {
		for (int m = 0; m < (int)PlayMode::MODE_MAX; ++m) {
			for (int i = 0; i < Scene::Manager::MAX_RECORD; ++i) {
				int score;
				if (ifs.read(reinterpret_cast<char*>(&score), sizeof(int))) {
					ranking[m][i] = score;
				}
			}
		}
		ifs.close();
	}
}