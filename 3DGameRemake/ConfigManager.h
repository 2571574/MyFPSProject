#pragma once
#include<map>
#include<vector>
#include<string>
#include"InputManager.h"

//ユーザー設定の構造
struct ConfigData {
	float bgmVolume = 0.5f;
	float seVolume = 0.5f;
	float mouseSensitivity = 0.050f;
	float padSensitivity = 0.005f;
	float basefov = 70.0f;
	bool headbob = true;
	bool recovery = false;
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

