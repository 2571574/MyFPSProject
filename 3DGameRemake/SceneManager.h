#pragma once
#include <memory>
#include "BaseScene.h"
class SceneManager
{
private:
	std::unique_ptr<BaseScene> currentScene;	//現在のシーン
	bool exitTag;
public:
	SceneManager();
	void ChangeScene(std::unique_ptr<BaseScene> nextscene);
	void Update();
	void Draw();

	bool GetExitTag() { return exitTag; }
};

