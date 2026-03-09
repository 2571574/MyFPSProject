#pragma once
#include <memory>
#include "BaseScene.h"

/// <summary>
///　全てのシーンを繋げるマネージャークラス
/// </summary>
class SceneManager
{
private:
	std::unique_ptr<BaseScene> currentScene;	//現在のシーン
	bool exitTag;		//終了タグ
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

	
	void SetExitTag(bool tag) { exitTag = tag; }
	bool GetExitTag() { return exitTag; }
};

