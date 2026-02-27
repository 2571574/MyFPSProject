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
	/// モデルの読み込みや初期化を行う
	/// </summary>
	virtual void Init() = 0;

	/// <summary>
	/// オブジェクトの更新や入力の処理を行う
	/// </summary>
	virtual void Update() = 0;

	/// <summary>
	/// 描画処理を行う
	/// </summary>
	virtual void Draw() = 0;
};

