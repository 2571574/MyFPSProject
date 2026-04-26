#pragma once
#include "BaseScene.h"
#include "SceneManager.h"

/// <summary>
/// リザルトシーンを管理するクラス
/// </summary>
class ResultScene :
    public BaseScene
{
private:
    int bgHandle;       //背景ハンドル

    int fontLarge = -1;  // リザルト見出し・スコア用
    int fontMedium = -1; // 各項目の情報用
    int fontSmall = -1;  // 下部の操作説明用

public:
    ResultScene(SceneManager* manager,int bgHandle = -1);
    ~ResultScene()override;

    void Init()override;
    void Update()override;
    void Draw()override;
};

