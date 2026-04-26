#pragma once
#include "BaseScene.h"
#include "Player.h"
#include "Camera.h"
#include "Dummy.h"
#include <vector>
#include <memory>

//チュートリアルの進行ステート
enum class TutorialPhase {
    MOVEMENT,
    COMBAT,
    FREERANGE
};

/// <summary>
/// チュートリアルを管理するクラス
/// </summary>
class TutorialScene :
    public BaseScene
{
private:
    Player player;  
    Camera camera;

	int stageHandle;     //ステージのモデルハンドル
    int fontLarge = -1;  // タイトル、PAUSE用
    int fontMedium = -1; // ダミー上のボタンテキスト用
    int fontSmall = -1;  // 説明文用

    bool isPaused;      //ポーズ中
    int pauseSelectNum; //ポーズメニューの選択肢番号
	enum PauseMenu { RESUME, RETURN_TITLE, PAUSE_MAX };  //ポーズメニューの選択肢

    void PauseUpdate();
    void PauseDraw();

    TutorialPhase currentPhase; //現在のステート

    //スポーンボタン(ダミーをボタンとして扱う)
    struct SpawnButton {
        std::unique_ptr<Dummy> dummy;
        int enemyType;
        VECTOR pos;
        float spawnCT;
    };
    std::vector<SpawnButton>button;

    int currentEnemyInfo;   //現在スポーンしている敵判別用

    std::vector<std::unique_ptr<Dummy>> target; //射撃場の的用ダミー


public:
    TutorialScene(SceneManager* manager);
    ~TutorialScene();

    void Init()override;
    void Update()override;
    void Draw()override;
};

