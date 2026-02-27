#pragma once
#include "DxLib.h"
#include"Parameter.h"
#include "Character.h"
#include "Time.h"

/*弾速を持つ武器の弾のクラス*/
class BaseProjectile {
protected:
	VECTOR startpos;	//発射位置
	VECTOR pos;			//現在座標
	VECTOR dir;			//発射方向
	GunStatus spec;		//武器性能
	TEAMID id;			//射手のチームID
	bool alive;			//生存タグ
public:
	//コンストラクタ
	BaseProjectile(VECTOR start,TEAMID _id,const GunStatus& _spec, VECTOR direction):startpos(start),pos(start), dir(direction), id(_id), spec(_spec), alive(true) {}
	
	//デストラクタ
	virtual ~BaseProjectile() {}

	void Update();	//更新
	void Draw();	//描画

	void Explode(VECTOR hitPos);
	//Getter
	GunStatus GetBulletStatus() const { return spec; }
	VECTOR GetPos() { return pos; }
	TEAMID GetID() { return id; }
	bool IsAlive() { return alive; }
};