#pragma once
#include "BaseScene.h"
#include "SceneManager.h"
class ResultScene :
    public BaseScene
{
public:
    ResultScene(SceneManager* manager);
    ~ResultScene()override;

    void Init()override;
    void Update()override;
    void Draw()override;
};

