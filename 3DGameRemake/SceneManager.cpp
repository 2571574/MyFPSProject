#include "SceneManager.h"
#include "GameScene.h"
SceneManager::SceneManager():exitTag(false) {
	ChangeScene(std::make_unique<GameScene>(this));
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