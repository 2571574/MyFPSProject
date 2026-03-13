#pragma once
#include "BaseScene.h"
#include "SceneManager.h"
#include<vector>
#include<string>
class TitleScene:public BaseScene
{
private:
	enum class TitleState {
		TOP,
		MODE_SELECT,
		SETTINGS,
		KEY_CONFIG,
		CREDIT,
	};

	enum PlayMenu {
		MENU_PLAY,
		MENU_SETTING,
		MENU_CREDIT,
		MENU_EXIT,
		MENU_MAX
	};

	TitleState currentState;
	int selectNum;

	void Control();
	
public:
	TitleScene(SceneManager* manager);
	~TitleScene()override;
	void Init()override;
	void Update()override;
	void Draw()override;
};