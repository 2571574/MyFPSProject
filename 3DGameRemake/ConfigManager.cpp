#include "ConfigManager.h"
#include <fstream>

ConfigManager::ConfigManager() { Load(); }

ConfigManager& ConfigManager::GetIns() {
	static ConfigManager ins;
	return ins;
}

void ConfigManager::Load() {
	std::ifstream ifs(fileName, std::ios::binary);
	//なければデフォルトをロードする
	if (!ifs) {
		InputManager::GetIns().SetDefaultBinding();
		bindings = InputManager::GetIns().GetBinds();
		return;
	}
	ifs.read((char*)&settings, sizeof(ConfigData));

	size_t mapSize;
	ifs.read((char*)&mapSize, sizeof(mapSize));
	for (size_t i = 0; i < mapSize; ++i) {
		ActionID id;
		size_t vecSize;
		ifs.read((char*)&id, sizeof(id));
		ifs.read((char*)&vecSize, sizeof(vecSize));
		std::vector<KeyBind> v(vecSize);
		ifs.read((char*)v.data(), sizeof(KeyBind) * vecSize);
		bindings[id] = v;
	}
}

void ConfigManager::Save() {
	std::ofstream ofs(fileName, std::ios::binary);

	ofs.write((char*)&settings, sizeof(ConfigData));

	size_t mapSize = bindings.size();
	ofs.write((char*)&mapSize, sizeof(mapSize));
	for (auto& pair : bindings) {
		ofs.write((char*)&pair.first, sizeof(pair.first));
		size_t vecSize = pair.second.size();
		ofs.write((char*)&vecSize, sizeof(vecSize));
		ofs.write((char*)pair.second.data(), sizeof(KeyBind) * vecSize);
	}
}