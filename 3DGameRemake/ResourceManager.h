#pragma once
#include "DxLib.h"

#include <string>
#include <unordered_map>

/// <summary>
///	モデルやサウンドなどのリソースを管理するクラス
/// </summary>
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
	std::unordered_map < std::string, int>movie;
	std::unordered_map < std::string, int>fonts;

public:
	static ResourceManager& GetIns();

	/// <summary>
	/// 3Dモデルのハンドルを取得する　ステージなどの単一オブジェクト用
	/// </summary>
	/// <param name="path">モデルのファイルパス</param>
	/// <returns>ハンドル</returns>
	int GetModel(const std::string& path);

	/// <summary>
	/// 3Dモデルを複製して取得する　武器や敵などのオブジェクト用
	/// MV1DeleteModel()の必要あり
	/// </summary>
	/// <param name="path">モデルのファイルパス</param>
	/// <returns>ハンドル</returns>
	int DuplicateModel(const std::string& path);

	/// <summary>
	/// 画像のハンドルを取得する
	/// </summary>
	/// <param name="path">画像のファイルパス</param>
	/// <returns>ハンドル</returns>
	int GetGraph(const std::string& path);

	/// <summary>
	/// 音声のハンドルを取得する
	/// </summary>
	/// <param name="path">音声のファイルパス</param>
	/// <returns>ハンドル</returns>
	int GetSound(const std::string& path);

	int GetFont(const std::string& path,int size,int thickness);


	void ClearAll();
};

