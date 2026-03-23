#pragma once
#include "DxLib.h"
#include <string>
enum class TEAMID {
	ID_FRIENDLY,
	ID_ENEMY,
};

enum class ENEMYTYPE {
	DUMMY,
	MELEE,
	RIFLE,
	SNIPER,
	ROLLING,
	MAX_TYPE
};

enum class PlayMode {
	MODE_TUTORIAL,
	MODE_EASY,
	MODE_NORMAL,
	MODE_HARD,
	MODE_MAX
};

enum class WeaponID {
	AR,
	SR,
	LR,
	SMG,
	PIS,
	ENEMY_KNIFE,
	ENEMY_AR,
	ENEMY_SR,
	ENEMY_EXPLOSION,

	UNKNOWN
};
//キャラのステータスの定義
struct CharacterStatus { //エンティティの性能
	int maxHP;			 //最大体力
	float accel;		 //加速度
	float friction;		 //摩擦係数
	int mass;			 //重量
	float width;		 //幅
	float height;		 //高さ
	float crouchHeight;	 //しゃがみ時の高さ
	float eyeHeight;	 //目の高さ
	float crouchEyeHeight;//しゃがみ時の目の高さ
	TEAMID teamID;		 //チームID
	int score;
};


struct WeaponVisual {
	std::string modelPath;
	float scale;
	VECTOR drawOffset;
	VECTOR adsDrawOffset;
};

struct GunStatus {		 //武器の性能
	WeaponID id;			//武器ID 
	bool hitscan;		//ヒットスキャン
	bool AOE;			//範囲攻撃武器
	bool fullAuto;		//フルオート
	bool isInfinite;	//弾薬無限
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
	VECTOR adsMuzzleOffset;	
	float hasDampingRatio;		//持った時の移動速度減衰率
	float adsDampingRatio;		//覗いた時の移動速度減衰率
	float adsFov;		 //覗いた時の視野角
	float projectileSize;	//弾のサイズ
	float projectileSpeed;	//弾速
	int explodeArea; //AOE=trueの場合　爆発範囲
	float knockbackP;
	bool friendlyFire;
	WeaponVisual visual;
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