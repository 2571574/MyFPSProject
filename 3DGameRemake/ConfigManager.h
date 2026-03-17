#pragma once
#include<map>
#include<vector>
#include<string>
#include"InputManager.h"

struct ConfigData {
	float bgmVolume = 0.5f;
	float seVolume = 0.5f;

	float mouseSensitivity = 0.1f;
	float padSensitivity = 0.002f;
	float basefov = 70.0f;
	bool headbob = true;
	bool recovery = false;
};
class ConfigManager
{
private:
	ConfigData settings;
	std::map<ActionID, std::vector<KeyBind>>bindings;
	const std::string fileName = "settings.dat";

	ConfigManager();
public:
	static ConfigManager& GetIns();

	void Save();
	void Load();

	ConfigData& Settings() { return settings; }
	std::map<ActionID, std::vector<KeyBind>>& Bindings() { return bindings; }
};

