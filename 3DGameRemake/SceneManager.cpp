#include "SceneManager.h"
#include "GameScene.h"
#include "TitleScene.h"

#include <fstream>
#include <algorithm>
#include <functional>

SceneManager::SceneManager()
	: exitTag(false)
	, currentMode(PlayMode::MODE_NORMAL){
	ranking.resize((int)PlayMode::MODE_MAX, std::vector<int>(MAX_RECORD, 0));
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
	lastResult.currentScore = score;

	if (currentMode == PlayMode::MODE_TUTORIAL) {
		return;
	}

	//ランキングを更新
	int modeIndex = (int)currentMode;
	ranking[modeIndex].push_back(score);
	std::sort(ranking[modeIndex].begin(), ranking[modeIndex].end(), std::greater<int>());
	if (ranking[modeIndex].size() > MAX_RECORD) {
		ranking[modeIndex].resize(MAX_RECORD);
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
			for (int i = 0; i < MAX_RECORD; ++i) {
				int score;
				if(ifs.read(reinterpret_cast<char*>(&score), sizeof(int))) {
					ranking[m][i] = score;
				}
			}
		}
		ifs.close();
	}
}