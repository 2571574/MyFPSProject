#pragma once

class SceneManager;

/// <summary>
/// 全シーンの基底クラス
/// </summary>
class BaseScene
{
protected:
	SceneManager* manager;

public:
	/// <summary>
	///　コンストラクタ SceneManagerのアドレスを保持
	/// </summary>
	/// <param name="_manager">SceneManagerのアドレス</param>
	BaseScene(SceneManager* _manager): manager(_manager){}
	virtual ~BaseScene(){}

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Init() = 0;

	/// <summary>
	/// 更新処理
	/// </summary>
	virtual void Update() = 0;

	/// <summary>
	/// 描画処理
	/// </summary>
	virtual void Draw() = 0;
};

