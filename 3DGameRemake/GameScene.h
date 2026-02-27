#pragma once
#include "BaseScene.h"
#include "CheckKey.h"
#include "Player.h"
#include "Time.h"
#include "ProjectileManager.h"
#include "Dummy.h"
#include "MeleeEnemy.h"
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
	
public:
	
	GameScene(SceneManager* manager);
	~GameScene() override;

	void Init() override;
	void Update()override;
	void Draw()override;
};

