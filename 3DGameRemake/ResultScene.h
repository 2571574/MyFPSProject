#pragma once
#include "BaseScene.h"
#include "SceneManager.h"

/// <summary>
/// リザルトシーンを管理するクラス
/// </summary>
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

