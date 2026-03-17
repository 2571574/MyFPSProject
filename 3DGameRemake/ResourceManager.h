#pragma once
#include "DxLib.h"
#include <string>
#include <unordered_map>
class ResourceManager
{
private:
	ResourceManager() = default;
	~ResourceManager() { ClearAll(); }

	ResourceManager(const ResourceManager&) = delete;
	ResourceManager& operator =(const ResourceManager&) = delete;

	std::unordered_map < std::string, int>models;
	std::unordered_map < std::string, int>sounds;
	std::unordered_map < std::string, int>graphics;

public:
	static ResourceManager& GetIns();

	int GetModel(const std::string& path);

	int DuplicateModel(const std::string& path);

	int GetGraph(const std::string& path);

	int GetSound(const std::string& path);
	void ClearAll();
};

