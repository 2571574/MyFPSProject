#pragma once
#include<map>
#include<vector>
#include<string>

#include "InputManager.h"
#include "Param/System.h"

//ユーザー設定の構造
struct ConfigData {
	float bgmVolume = System::Config::DEFAULT_BGM_VOLUME;
	float seVolume = System::Config::DEFAULT_SE_VOLUME;
	float mouseSensitivity = System::Config::DEFAULT_MOUSE_SENSITIVITY;
	float padSensitivity = System::Config::DEFAULT_PAD_SENSITIVITY;
	float basefov = System::Config::DEFAULT_BASE_FOV;
	bool headbob = System::Config::DEFAULT_HEADBOB;
	bool recovery = System::Config::DEFAULT_RECOVERY;
};

/// <summary>
/// ユーザー設定とキーバインドを保存、読み込むクラス
/// </summary>
class ConfigManager
{
private:
	ConfigData settings;	//設定
	std::map<ActionID, std::vector<KeyBind>>bindings;	//バインド
	const std::string fileName = "settings.dat";	//ファイル名

	ConfigManager();
public:
	static ConfigManager& GetIns();

	void Save();
	void Load();

	ConfigData& Settings() { return settings; }
	std::map<ActionID, std::vector<KeyBind>>& Bindings() { return bindings; }
};

