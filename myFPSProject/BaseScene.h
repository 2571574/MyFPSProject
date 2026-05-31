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
	BaseScene(SceneManager* _manager): manager(_manager){}
	virtual ~BaseScene(){}

	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;
};