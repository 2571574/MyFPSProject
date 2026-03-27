#pragma once
#pragma message(">>> Using Debug.h from: " __FILE__)
#include"DxLib.h"
#include<string>
#include<vector>
#include<utility>

class Debug {
private:
	static bool s_enabled;	//デバッグモードの有効無効

	static std::vector < std::string>s_logList;
	static std::vector < std::pair < std::string, std::string>> s_watchList;//監視変数

	static int s_logPosX, s_logPosY;
	static int s_watchPosX, s_watchPosY;

	static void SetWatchString(const std::string& name, const std::string& value);

	static constexpr int BOX_WIDTH = 250;
	static constexpr int BOX_SPEED = 4;
	static constexpr int BGALPHA = 80;
public:
	static void SetEnabled(bool tag);
	static bool IsEnabled();

	static void Update();

	//ログ出力
	static void Log(const std::string& text);

	//値を監視する
	template<typename T>
	static void Watch(const std::string& name, const T& value) {
		if (!s_enabled) return;
		SetWatchString(name, std::to_string(value));
	}

	static void Watch(const std::string& name, bool value) {
		if (!s_enabled) return;
		SetWatchString(name, value ? "true" : "false");
	}

	static void Watch(const std::string& name, const std::string& value);
	static void Watch(const std::string& name, const char* value);


	//画面に情報を描画する
	static void Draw();

	//リセット
	static void Clear();
};