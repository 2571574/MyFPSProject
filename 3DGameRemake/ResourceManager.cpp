#include "ResourceManager.h"
ResourceManager& ResourceManager::GetIns() {
	static ResourceManager instance;
	return instance;
}

int ResourceManager::GetModel(const std::string& path) {
	if (models.find(path) != models.end()) {
		return models[path];
	}

	int handle = MV1LoadModel(path.c_str());
	if (handle != -1) {
		models[path] = handle;
	}
	return handle;
}

int ResourceManager::DuplicateModel(const std::string& path) {
	int masterHandle = GetModel(path);
	if (masterHandle == -1) return -1;
	return MV1DuplicateModel(masterHandle);
}


int ResourceManager::GetGraph(const std::string& path) {
	if (graphics.find(path) != graphics.end()) {
		return graphics[path];
	}

	int handle = LoadGraph(path.c_str());
	if (handle != -1) {
		graphics[path] = handle;
	}
	return handle;
}

int ResourceManager::GetSound(const std::string& path) {
	if (sounds.find(path) != sounds.end()) {
		return sounds[path];
	}

	int handle = LoadSoundMem(path.c_str());
	if (handle != -1) {
		sounds[path] = handle;
	}
	return handle;
}

int ResourceManager::GetFont(const std::string& path, int size, int thickness) {
	std::string key = path + std::to_string(size) + std::to_string(thickness);
	if (fonts.find(key) != fonts.end()) {
		return fonts[key];
	}

	int handle = CreateFontToHandle(path.c_str(), size, thickness, DX_FONTTYPE_ANTIALIASING_8X8);
	if (handle != -1) {
		fonts[key] = handle;
	}
	return handle;
}

void ResourceManager::ClearAll() {
	for (auto& pair : models) {
		MV1DeleteModel(pair.second);
	}
	models.clear();

	for (auto& pair : graphics) {
		DeleteGraph(pair.second);
	}
	graphics.clear();
	for (auto& pair : sounds) {
		DeleteSoundMem(pair.second);
	}
	sounds.clear();
	for (auto& pair : fonts) {
		DeleteFontToHandle(pair.second);
	}
	fonts.clear();
}