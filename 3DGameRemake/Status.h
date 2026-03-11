#pragma once
#include "DxLib.h"

enum class TEAMID {
	ID_FRIENDLY,
	ID_ENEMY,
};



//キャラのステータスの定義
struct CharacterStatus { //エンティティの性能
	int maxHP;			 //最大体力
	float accel;		 //加速度
	float friction;		 //摩擦係数
	float width;		 //幅
	float height;		 //高さ
	float crouchHeight;
	float eyeHeight;
	float crouchEyeHeight;
	TEAMID teamID;		 //チームID
};

struct GunStatus {		 //武器の性能
	bool hitscan;		//ヒットスキャン
	bool AOE;			//範囲攻撃武器
	bool fullAuto;		//フルオート
	int damage;			 //ダメージ
	float range;		 //射程
	int magAmmo;		 //弾数
	int bagAmmo;		 //予備弾数
	float reloadTime;	 //リロード時間
	float fireRate;		 //連射速度
	float spread;		 //拡散率
	float adsSpread;
	float recoil;	     //反動
	VECTOR muzzleOffset; //銃口のオフセット
	float hasDampingRatio;		//持った時の移動速度減衰率
	float adsDampingRatio;		//覗いた時の移動速度減衰率
	float adsFov;		 //覗いた時の視野角
	float projectileSize = 0;	//弾のサイズ
	float projectileSpeed = 0;	//弾速
	int explodeArea = 0; //AOE=trueの場合　爆発範囲
};

//ステータスのテンプレート
namespace CHARA_STATUS {
	extern CharacterStatus PLAYER;
	extern CharacterStatus DUMMY;
	extern CharacterStatus MELEE_ENEMY;
	extern CharacterStatus RIFLE_ENEMY;
	extern CharacterStatus SNIPER_ENEMY;
	extern CharacterStatus ROLL_ENEMY;
}
namespace PLAYER_GUN {
	extern GunStatus RIFLE;
	extern GunStatus SNIPER;
	extern GunStatus LAUNCHER;
	extern GunStatus SMG;
	extern GunStatus PISTOL;
}
namespace ENEMY_GUN {
	extern GunStatus MELEE;
	extern GunStatus RIFLE;
	extern GunStatus SNIPER;
	extern GunStatus DESTRUCT;
}