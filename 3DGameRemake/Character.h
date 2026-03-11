#pragma once
#include "DxLib.h"
#include "Status.h"
#include "Parameter.h"
/// <summary>
/// ゲーム内キャラすべての基底クラス
/// </summary>
class Character
{
protected:
	VECTOR position;		//座標
	VECTOR velocity;		//速度
	CharacterStatus status;	//ステータス情報
	bool alive;				//生存タグ
	int hp;					//現在HP
	bool crouch;		//しゃがみ
	float currentHeight;
	float currentEyeHeight;	//現在の目の高さ

	int modelHandle;		//モデルハンドル

public:
	/// <summary>
	/// コンストラクタ　位置とステータスを受け取る
	/// </summary>
	/// <param name="_position">初期座標</param>
	/// <param name="_status">キャラのステータス</param>
	Character(VECTOR _position, CharacterStatus& _status) :
		position(_position)
		, velocity({ 0,0,0 })
		, status(_status)
		, hp(_status.maxHP)
		, alive(true)
		, crouch(false)
		, currentHeight(_status.height)
		, currentEyeHeight(_status.eyeHeight)
		, modelHandle(-1) {};		//コンストラクタ

	virtual ~Character() {if(modelHandle!= -1) MV1DeleteModel(modelHandle);}

	/// <summary>
	/// 更新　派生クラスでオーバーライド
	/// </summary>
	virtual void Update() = 0;

	/// <summary>
	/// 描画　派生クラスでオーバーライド
	/// </summary>
	virtual void Draw() = 0;		//描画

	/// <summary>
	/// 被弾処理　ダメージ量分HPを減らし、死亡判定も行う
	/// </summary>
	/// <param name="damage">ダメージ量</param>
	virtual void TakeDamage(int damage) {
		if (!alive)return;		//死亡ならreturn 
		hp -= damage;		//ダメージ分HPを減らす

		//HPが0以下なら死亡判定
		if (hp <= 0) {
			hp = 0;
			alive = false;
		}
	}

	/// <summary>
	/// リコイル分のカメラの揺れを加える　プレイヤー用
	/// </summary>
	/// <param name="y">横反動の量</param>
	/// <param name="p">縦反動の量</param>
	virtual void  AddRecoil(float y, float p){}

	//getter
	bool IsCrouching() { return crouch; }
	bool IsAlive() const { return alive; }
	int GetHP()const { return hp; }
	VECTOR GetPos()const { return position; }
	CharacterStatus GetStatus()const { return status; }
	TEAMID GetID()const { return status.teamID; }
	float GetCurrentHeight()const { return currentHeight; }
	float GetCurrentEyeHeight()const { return currentEyeHeight; }
};
