#pragma once
#include "BaseScene.h"
#include "CheckKey.h"
#include "Player.h"
#include "Time.h"
#include "ProjectileManager.h"
#include "Dummy.h"
#include "MeleeEnemy.h"
#include "RifleEnemy.h"
#include "SniperEnemy.h"
#include "RollingEnemy.h"
#include "EnemyManager.h"

/// <summary>
/// ゲームシーンを管理するクラス
/// </summary>
class GameScene: public BaseScene
{
private:
	Player player;		//プレイヤー
	Camera camera;		//カメラ
	int stageHandle;	//ステージのモデルハンドル
	int score;

	bool isPaused;
	int pauseSelectNum;
	enum PauseMenu {
		RESUME,
		RETURN_TITLE,
		PAUSE_MAX
	};
	void PauseUpdate();
	void PauseDraw();
public:
	
	GameScene(SceneManager* manager);
	~GameScene() override;

	/// <summary>
	/// 初期化　モデルの読み込みや位置の設定など
	/// </summary>
	void Init() override;

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update()override;

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw()override;
};

