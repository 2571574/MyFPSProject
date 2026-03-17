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
	int score;			//スコア

	bool isPaused;		//ポーズ中フラグ
	int pauseSelectNum;//ポーズメニュー選択

	//ポーズメニュー
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

	void Init() override;
	void Update()override;
	void Draw()override;
};

