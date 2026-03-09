#pragma once
#include "DxLib.h"
#include"Parameter.h"
#include "Character.h"
#include "Time.h"

/// <summary>
/// プロジェクタイルの弾を管理するクラス
/// </summary>
class BaseProjectile {
protected:
	VECTOR startpos;	//発射位置
	VECTOR pos;			//現在座標
	VECTOR dir;			//発射方向
	GunStatus spec;		//武器性能
	TEAMID id;			//射手のチームID
	bool alive;			//生存タグ
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="start">発射位置</param>
	/// <param name="_id">射手のチームID</param>
	/// <param name="_spec">撃った銃の性能</param>
	/// <param name="direction">射撃方向のベクトル</param>
	BaseProjectile(VECTOR start,TEAMID _id,const GunStatus& _spec, VECTOR direction):startpos(start),pos(start), dir(direction), id(_id), spec(_spec), alive(true) {
		dir = VNorm(dir);
	}
	
	
	virtual ~BaseProjectile() {}

	/// <summary>
	/// 更新処理
	/// </summary>
	virtual void Update();

	/// <summary>
	/// 描画処理
	/// </summary>
	virtual void Draw();

	/// <summary>
	/// 着弾点で爆発させる　AOE=trueの武器の場合のみ
	/// </summary>
	/// <param name="hitPos">着弾位置</param>
	virtual void Explode(VECTOR hitPos);

	//Getter
	const GunStatus& GetBulletStatus() const { return spec; }
	VECTOR GetPos() const{ return pos; }
	TEAMID GetID() const{ return id; }
	bool IsAlive() const{ return alive; }
};