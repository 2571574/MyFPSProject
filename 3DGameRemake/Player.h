#pragma once
#include "Character.h"
#include "Camera.h"
#include "InputManager.h"
#include "Weapon.h"
#include "HUD.h"

#include <memory>
#include <vector>


/// <summary>
/// プレイヤーを管理するクラス
/// </summary>
class Player : public Character
{
private:
	VECTOR forwardVec, rightVec;		//プレイヤーの方向から取った前ベクトル、右ベクトル
	std::vector<std::unique_ptr<Weapon>> slot;		//現在持っている武器	
	int currentWeaponIndex;
	int maxWeaponSlot;
	float fov;			//現在の視野角
	bool isAds;			//ADSしているか
	float slidingCT;	//スライディングのクールタイム
	bool running;		//走っているか
	bool headBob;		//歩行時のカメラの揺れをonにするか
	float bobbingTimer; //カメラの揺れのタイマー
	Camera* cam;		//カメラのポインタ
	std::unique_ptr<HUD> hud;	//HUD


	int Shot = 0;
	int totalHit = 0;
	int totalHeadHit = 0;

	int stageHandle;	//ステージのモデルハンドル
	std::vector<VECTOR> TargetedPos;

	PlayMode currentMode;
public:
	/// <summary>
	///	playerのコンストラクタ 座標にplayerを生成
	/// </summary>
	/// <param name="pos">playerの初期座標</param>
	Player(VECTOR pos, Camera* camera, PlayMode mode);	//コンストラクタ

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


	void SwitchWeapon(int next);
	bool AddWeapon(std::unique_ptr<Weapon>& newWeapon);

	void ShotRecord()override { Shot++; }
	void HitRecord(bool isHeadShot)override {
		totalHit++;
		if (isHeadShot)totalHeadHit++;

		if (hud) {
			hud->OnHitTarget(isHeadShot);
		}
	}
	//getter



	void SetStageHandle(int handle) { stageHandle = handle; }

	Weapon* GetWeapon() const { 
		if (slot.empty() || currentWeaponIndex < 0 || currentWeaponIndex >= slot.size()) return nullptr;
		return slot[currentWeaponIndex].get();
	}
	int GetWeaponIndex()const { return currentWeaponIndex; }

	void AddTargeted(VECTOR pos) { TargetedPos.push_back(pos); }
	const std::vector<VECTOR>& GetTargeted()const { return TargetedPos; }
	void ClearTargeted() { TargetedPos.clear();}

	/// <summary>
	/// 現在のカメラの注視方向（視線ベクトル）を取得
	/// </summary>
	/// <returns>カメラの注視方向を返す</returns>
	VECTOR GetCamDirection()const;
	Camera* GetCam()const { return cam; }

	int GetShots()const { return Shot; }
	int GetHits()const { return totalHit; }
	int GetHeadShot()const { return totalHeadHit; }
};

