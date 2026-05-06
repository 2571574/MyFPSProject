#pragma once
#include "Character.h"
#include "Camera.h"
#include "InputManager.h"
#include "Weapon.h"
#include "HUD.h"

#include <memory>
#include <vector>

struct TargetInfo {
	VECTOR pos;
	float progress;
};

/// <summary>
/// プレイヤーを管理するクラス
/// </summary>
class Player : public Character
{
private:
	VECTOR forwardVec, rightVec;		//プレイヤーの方向から取った前ベクトル、右ベクトル
	float fov;			//現在の視野角
	bool isAds;			//ADSしているか
	float slidingCT;	//スライディングのクールタイム
	bool running;		//走っているか
	bool headBob;		//歩行時のカメラの揺れをonにするか
	float bobbingTimer; //カメラの揺れのタイマー
	bool isWallRunning;     // 壁走り中か
	int wallRunDir;         // 壁の方向
	VECTOR wallNormal;      // 現在接している壁の法線（ジャンプ時の反射計算用）
	float currentRoll;      // 現在のカメラの傾き
	std::unique_ptr<HUD> hud;	//HUD

	std::vector<std::unique_ptr<Weapon>> slot;		//現在持っている武器	
	int currentWeaponIndex;		//現在所持している武器のスロット番号
	int maxWeaponSlot;			//最大武器スロット数

	WeaponID lastHit = WeaponID::UNKNOWN;	//最後の被弾武器
	int Shot = 0;			//射撃回数
	int totalHit = 0;		//命中回数
	int totalHeadHit = 0;	//ヘッドショット回数

	int stageHandle;	//ステージのモデルハンドル

	std::vector<TargetInfo> targetInfo;	//インジケーター用ターゲット情報

	PlayMode currentMode;	//現在の難易度

	Camera* cam;		//カメラのポインタ
	float lastCamYaw;	//前回のカメラの水平角
	float lastCamPitch;	//前回のカメラの垂直角
	float currentSwayX;	//現在の武器スウェイX
	float currentSwayY;	//現在の武器スウェイY

	float moveDistance;//移動距離カウント
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

	/// <summary>
	/// 武器を切り替える
	/// </summary>
	/// <param name="next">切替先の番号</param>
	void SwitchWeapon(int next);

	/// <summary>
	/// 武器を追加する
	/// </summary>
	/// <param name="newWeapon">拾う武器</param>
	/// <returns>取得処理が成功したらtrue</returns>
	bool AddWeapon(std::unique_ptr<Weapon>& newWeapon);

	void ShotRecord()override { Shot++; }
	void HitRecord(bool isHeadShot, bool isKill = false)override {
		totalHit++;
		if (isHeadShot)totalHeadHit++;

		if (hud) {
			hud->OnHitTarget(isHeadShot,isKill);
		}
	}
	
	void OnHit(int damage, WeaponID id = ::WeaponID::UNKNOWN)override {
		Character::OnHit(damage, id);
		if (damage > 0) {
			lastHit = id;
			if (hud) hud->OnPlayerTakeDamage();
		}
	}

	/// <summary>
	/// カメラの角度と武器スウェイの状態を同期させる
	/// </summary>
	void SyncCamAngle() {
		if (cam) {
			lastCamYaw = cam->GetYaw();
			lastCamPitch = cam->GetPitch();
			currentSwayX = 0.0f;
			currentSwayY = 0.0f;
		}
	}

	/// <summary>
	/// 移動距離に基づき足音を鳴らす
	/// </summary>
	void UpdateFootstep();

	void SetStageHandle(int handle) { stageHandle = handle; }

	Weapon* GetWeapon() const { 
		if (slot.empty() || currentWeaponIndex < 0 || currentWeaponIndex >= slot.size()) return nullptr;
		return slot[currentWeaponIndex].get();
	}
	int GetWeaponIndex()const { return currentWeaponIndex; }
	PlayMode GetCurrentMode() const { return currentMode; }
	void AddTargeted(VECTOR pos, float progress) { targetInfo.push_back({ pos, progress }); }
	const std::vector<TargetInfo>& GetTargeted()const { return targetInfo; }
	void ClearTargeted() { targetInfo.clear();}
	VECTOR GetCamDirection()const;
	Camera* GetCam()const { return cam; }
	int GetShots()const { return Shot; }
	int GetHits()const { return totalHit; }
	int GetHeadShot()const { return totalHeadHit; }
	WeaponID GetLastHitWeapon()const { return lastHit; }
};

