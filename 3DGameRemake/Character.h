#pragma once
#include "DxLib.h"
#include "Status.h"
#include "Parameter.h"
#include "CollisionManager.h"
/// <summary>
/// ゲーム内キャラすべての基底クラス
/// </summary>
class Character
{
protected:
	VECTOR position;		//座標
	VECTOR velocity;		//速度
	VECTOR knockback;		//ノックバックによる加速
	CharacterStatus status;	//ステータス情報
	bool alive;				//生存タグ
	int hp;					//現在HP
	bool crouch;		//しゃがみ
	bool onGround;		//地面にいるか
	float currentHeight;	//現在の高さ
	float currentEyeHeight;	//現在の目の高さ


	/// <summary>
	/// 被弾処理　ダメージ量分HPを減らす
	/// </summary>
	/// <param name="damage">ダメージ量</param>
	virtual void TakeDamage(int damage,WeaponID id = WeaponID::UNKNOWN) {
		if (!alive)return;		//死亡ならreturn 
		hp -= damage;		//ダメージ分HPを減らす
	}

public:
	/// <summary>
	/// コンストラクタ　位置とステータスを受け取る
	/// </summary>
	/// <param name="_position">初期座標</param>
	/// <param name="_status">キャラのステータス</param>
	Character(VECTOR _position, const CharacterStatus& _status);

	virtual ~Character();

	/// <summary>
	/// 更新　派生クラスでオーバーライド
	/// </summary>
	virtual void Update() = 0;

	/// <summary>
	/// 描画　派生クラスでオーバーライド
	/// </summary>
	virtual void Draw() = 0;		//描画

	/// <summary>
	/// 被弾処理
	/// </summary>
	/// <param name="damage">喰らったダメージ量</param>
	virtual void OnHit(int damage,WeaponID id = WeaponID::UNKNOWN) {
		if (!alive) return;
		TakeDamage(damage,id);
	}

	/// <summary>
	/// リコイル分のカメラの揺れを加える　プレイヤー用
	/// </summary>
	/// <param name="y">横反動の量</param>
	/// <param name="p">縦反動の量</param>
	virtual void  AddRecoil(float y, float p){}

	/// <summary>
	/// 移動と当たり判定の押し出しを適用させる
	/// </summary>
	/// <param name="moveDir">移動方向</param>
	/// <param name="stageHandle">ステージのモデルハンドル</param>
	virtual void ApplyMovement(VECTOR moveDir, int stageHandle);

	/// <summary>
	/// 速度を更新する
	/// </summary>
	/// <param name="moveDir">移動方向</param>
	virtual void UpdateVelocity(VECTOR moveDir);

	/// <summary>
	/// 床や壁の当たり判定をチェックする
	/// </summary>
	/// <param name="stagehandle">ステージのモデルハンドル</param>
	virtual void UpdatePhysics(int stagehandle);

	/// <summary>
	/// 体力を全快させる
	/// </summary>
	virtual void revive() {
		hp = status.maxHP;
		alive = true;
	}

	/// <summary>
	/// ノックバックを適用する
	/// </summary>
	/// <param name="force">ノックバックの強さ</param>
	void Applyknockback(VECTOR force) {
		knockback.x += force.x;
		knockback.z += force.z;
		velocity.y += force.y;
	}

	/// <summary>
	/// 射撃をカウントする
	/// </summary>
	virtual void ShotRecord(){}

	/// <summary>
	/// 命中をカウントする
	/// </summary>
	/// <param name="isHeadShot">ヘッドショット</param>
	virtual void HitRecord(bool isHeadShot){}

	/// <summary>
	/// 壁との当たり判定をチェックする
	/// </summary>
	/// <param name="stagehandle"></param>
	void ResolveWallPenetration(int stagehandle);

	void SetPos(VECTOR _pos) { position = _pos; }

	bool IsCrouching() const { return crouch; }
	bool IsAlive() const { return alive; }
	int GetHP()const { return hp; }
	VECTOR GetPos()const { return position; }
	CharacterStatus GetStatus()const { return status; }
	TEAMID GetID()const { return status.teamID; }
	float GetCurrentHeight()const { return currentHeight; }
	float GetCurrentEyeHeight()const { return currentEyeHeight; }
	VECTOR GetVelocity()const { return velocity; }
};
