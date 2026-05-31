#pragma once
#include "BaseScene.h"
#include "CheckKey.h"
#include "Time.h"
#include "EnemyManager.h"
#include "ProjectileManager.h"
#include "Player.h"
#include "Dummy.h"
#include "MeleeEnemy.h"
#include "RifleEnemy.h"
#include "SniperEnemy.h"
#include "RollingEnemy.h"

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
	
	bool isDeadSequence = false;	//死亡アニメーション中
	float deathTimer = 0.0f;		//死亡アニメーションタイマー

	bool isSceneChange = false;		//シーン遷移中
	bool reqTransition = false;		//リザルト遷移の要求

	int monochromeHandle = -1;		//モノクロ画像ハンドル
	int fontLarge = -1;				//ポーズタイトル
	int fontMedium = -1;			//ポーズ選択肢

	void PauseUpdate();
	void PauseDraw();
public:
	
	GameScene(SceneManager* manager);
	~GameScene() override;

	void Init() override;
	void Update()override;
	void Draw()override;
};

