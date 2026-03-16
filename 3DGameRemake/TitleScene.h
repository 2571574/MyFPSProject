#pragma once
#include "BaseScene.h"
#include "SceneManager.h"
#include "InputManager.h"
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

	enum class SettingItem {
		MOUSE_SENSITIVITY,
		PAD_SENSITIVITY,
		BASE_FOV,
		HEAD_BOB,
		RECOVERY,
		KEY_CONFIG,
		BACK,
		MAX
	};

	TitleState currentState;
	int selectNum;
	int columnidx = 0;
	bool isWaitingKey = false;


	void Control();
	void ChangeState(TitleState next);
	void RemoveDuplicateBind(InputType type,int code);
	void ModifySetting(SettingItem item, int direction);

	void DrawSettings();
	void DrawKeyConfig();
public:
	TitleScene(SceneManager* manager);
	~TitleScene()override;
	void Init()override;
	void Update()override;
	void Draw()override;

};