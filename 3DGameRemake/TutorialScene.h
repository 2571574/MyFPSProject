#pragma once
#include "BaseScene.h"
#include "Player.h"
#include "Camera.h"
#include "Dummy.h"
#include <vector>
#include <memory>

enum class TutorialPhase {
    MOVEMENT,
    COMBAT,
    FREERANGE
};


class TutorialScene :
    public BaseScene
{
private:
    Player player;
    Camera camera;

    int stageHandle;

    bool isPaused;
    int pauseSelectNum;
    enum PauseMenu{RESUME,RETURN_TITLE,PAUSE_MAX};
    void PauseUpdate();
    void PauseDraw();

    TutorialPhase currentPhase;

    struct SpawnButton {
        std::unique_ptr<Dummy> dummy;
        int enemyType;
        VECTOR pos;
    };
    std::vector<SpawnButton>button;
    std::unique_ptr<Character> currentEnemy;
    int currentEnemyInfo;

    std::vector<std::unique_ptr<Dummy>> target;
public:
    TutorialScene(SceneManager* manager);
    ~TutorialScene();

    void Init()override;
    void Update()override;
    void Draw()override;
};

