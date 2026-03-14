#include "SceneManager.h"
#include "GameScene.h"
#include "TitleScene.h"
#include <fstream>
#include <algorithm>
SceneManager::SceneManager():exitTag(false) {
	ranking.resize(PlayMode::MODE_MAX, std::vector<int>(5, 0));
	LoadRanking();
	ChangeScene(std::make_unique<TitleScene>(this));
}

void SceneManager::ChangeScene(std::unique_ptr<BaseScene> nextscene) {
	currentScene = std::move(nextscene);
	currentScene->Init();
}

void SceneManager::Update() {
	if (currentScene) {
		currentScene->Update();
	}
}

void SceneManager::Draw() {
	if (currentScene) {
		currentScene->Draw();
	}
}

void SceneManager::SetScore(int score) {
	currentScore = score;
	if (currentMode == PlayMode::MODE_TUTORIAL) {
		return;
	}
	ranking[currentMode].push_back(currentScore);
	std::sort(ranking[currentMode].begin(), ranking[currentMode].end(), std::greater<int>());
	if (ranking[currentMode].size() > 5) {
		ranking[currentMode].resize(5);
	}
	SaveRanking();
}

void SceneManager::SaveRanking() {
	std::ofstream ofs(RANKING_FILE, std::ios::binary | std::ios::trunc);
	if (ofs.is_open()) {
		for (int m = 0; m < PlayMode::MODE_MAX; ++m) {
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
		for (int m = 0; m < PlayMode::MODE_MAX; ++m) {
			for (int i = 0; i < 5; ++i) {
				int score;
				if(ifs.read(reinterpret_cast<char*>(&score), sizeof(int))) {
					ranking[m][i] = score;
				}
			}
		}
		ifs.close();
	}
}