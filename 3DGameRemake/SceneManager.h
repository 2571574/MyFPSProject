#pragma once
#include <memory>
#include "BaseScene.h"
#include "Status.h"

/// <summary>
///　全てのシーンを繋げるマネージャークラス
/// </summary>
class SceneManager
{
private:
	std::unique_ptr<BaseScene> currentScene;	//現在のシーン
	bool exitTag;		//終了タグ
	PlayMode currentMode;
public:
	SceneManager();

	/// <summary>
	/// 次のシーンに切り替える
	/// </summary>
	/// <param name="nextscene">次のシーンのインスタンス</param>
	void ChangeScene(std::unique_ptr<BaseScene> nextscene);

	/// <summary>
	/// 更新処理　現在のシーンの更新処理を呼び出す
	/// </summary>
	void Update();

	/// <summary>
	/// 描画処理　現在のシーンの描画処理を呼び出す
	/// </summary>
	void Draw();

	void SetPlayMode(PlayMode mode) { currentMode = mode; }
	PlayMode GetcurrentMode()const { return currentMode; }
	void SetExitTag(bool tag) { exitTag = tag; }
	bool GetExitTag() { return exitTag; }
};

