#pragma once
#include "Character.h"
#include "Camera.h"
#include "InputManager.h"
#include "Weapon.h"
#include <memory>

/*プレイヤーを管理するクラス*/
class Player : public Character
{
private:
	VECTOR forwardVec, rightVec;	//プレイヤーの方向から取った前ベクトル、右ベクトル
	std::unique_ptr<Weapon> weapon;		
	float fov;
	bool isAds;			
	bool onGround;
	float slidingCT;
	bool running;
	bool headBob;
	float bobbingTimer;
	Camera* cam;

	int stageHandle;
public:
	/// <summary>
	///	playerのコンストラクタ 座標にplayerを生成
	/// </summary>
	/// <param name="pos">playerの初期座標</param>
	Player(VECTOR pos,Camera* camera);	//コンストラクタ

	/// <summary>
	/// playerのデストラクタ　使用していたハンドルの削除
	/// </summary>
	~Player() override;
	
	/// <summary>	
	/// playerの更新	
	/// </summary>
	void Update() override;

	/// <summary>
	/// playerの描画
	/// </summary>
	void Draw() override;	//描画

	/// <summary>
	/// リコイル（反動）を加える。
	/// </summary>
	/// <param name="y">横方向の反動量</param>
	/// <param name="p">縦方向の反動量</param>
	void AddRecoil(float y, float p)override;
	//getter

	/// <summary>
	/// ステージのモデルをセットする
	/// </summary>
	/// <param name="handle">ステージモデルのハンドル</param>
	void SetStageHandle(int handle) { stageHandle = handle; }

	/// <summary>
	/// 現在のカメラの注視方向（視線ベクトル）を取得
	/// </summary>
	/// <returns>カメラの注視方向を返す</returns>
	VECTOR GetCamDirection();

	/// <summary>
	/// 現在持っている武器を取得
	/// </summary>
	/// <returns>持っている武器のポインタを返す</returns>
	Weapon* GetWeapon() { return weapon.get(); }
};

