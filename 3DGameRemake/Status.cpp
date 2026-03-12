#include "Status.h"
namespace CHARA_STATUS {
	CharacterStatus PLAYER = { 100, 0.015f, 0.9f, 5, 0.5f, 2.0f, 1.0f,1.8f,0.8f,TEAMID::ID_FRIENDLY, 0};
	CharacterStatus DUMMY = { 10000,0.0f,0.0f, 10, 1.0f,2.0f,2.0f,1.8f,1.8f, TEAMID::ID_ENEMY, 0 };
	CharacterStatus MELEE_ENEMY = { 1,0.023f,0.8f, 1, 0.5f,2.0f,2.0f,1.8f,1.8f,TEAMID::ID_ENEMY , 100};
	CharacterStatus RIFLE_ENEMY = { 1,0.012f,0.8f, 1, 0.5f,2.0f,2.0f,1.8f,1.8f,TEAMID::ID_ENEMY , 150};
	CharacterStatus SNIPER_ENEMY = { 1,0.01f,0.8f, 1, 0.5f,1.5f,1.5f,1.3f,1.3f,TEAMID::ID_ENEMY , 150};
	CharacterStatus ROLL_ENEMY = { 1, 0.01f, 0.95f, 1, 0.5f,0.5f, 0.5f, 0.25f, 0.25f, TEAMID::ID_ENEMY, 50};
}
namespace PLAYER_GUN {
	GunStatus RIFLE = { true, false,true, 1, 30.0f, 20, 120, 2.0f, 12.0f, 0.15f, 0.005f, 2.0f, VGet(0.3f,-0.2f,0.0f), 1.0f,0.4f,50.0f};
	GunStatus SNIPER = { true, false,false, 1, 60.0f, 3, 40, 3.0f, 0.6f, 0.2f, 0.0001f, 7.0f,VGet(0.3f,-0.2f,0.0f), 0.9f, 0.2f, 40.0f };
	GunStatus LAUNCHER = { false, true, false, 2, 20.0f, 1, 5, 1.5f, 10.0f, 0.001f,0.001f, 14.0f,VGet(0.3f,-0.2f,0.0f),0.5f,0.5f,65.0f,0.1f,0.5f,10.0f };
	GunStatus SMG = { false, false, true, 1, 30.0f, 30, 180, 1.8f, 20.0f, 0.1f, 0.03f, 1.5f, VGet(0.3f,-0.2f,0.0f), 1.0f, 0.7f, 60.0f,0.06f,1.0f };
	GunStatus PISTOL = { false, false, false, 1, 20.0f, 8, 0, 2.0f, 8.0f, 0.1f, 0.01f, 3.0f,VGet(0.3f,-0.2f,0.0f),1.0f,0.8f,60.0f,0.06f,1.0f };
}
namespace ENEMY_GUN {
	GunStatus RIFLE = { false, false, true, 1, 20.0f, 20, 0, 5.0f, 1.0f, 0.001f, 0.001f, 0.0f, VGet(0.0f,0.0f,0.0f), 1.0f,1.0f,50.0f,0.1f,0.3f };
	GunStatus SNIPER = { false, false, true, 1, 30.0f, 3, 0, 10.0f, 0.2f, 0.0001f, 0.0001f, 0.0f,VGet(0.0f,0.0f,0.0f), 1.0f, 0.5f, 40.0f, 0.06f, 0.8f };
	GunStatus DESTRUCT = { false,false,false,10, 0.0f, 0, 0, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, VGet(0.0f, 0.0f, 0.0f), 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 10.0f };
}
/*
  Entity						Gun
  最大HP,						ヒットスキャンかどうか,
  加速度,						着弾で爆発かどうか,
  摩擦係数,						フルオート,
								ダメージ,
  幅,							射程,
  高さ,							弾倉の最大弾数,
  チームID,						予備弾数,
								リロードの時間,
								連射速度,
								拡散率,
								ads時の拡散率
								反動,
								銃口のオフセット,
								持った時の移動速度減衰率
								ads時の移動速度の減衰率
								ads時の視野角,
								弾のサイズ,
								弾速,
								範囲武器爆発範囲,
*/