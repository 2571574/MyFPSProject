#include "Character.h"
#include "CollisionManager.h"

/// <summary>
/// コンストラクタ　位置とステータスを受け取る
/// </summary>
/// <param name="_position">初期座標</param>
/// <param name="_status">キャラのステータス</param>
Character::Character(VECTOR _position, CharacterStatus& _status) :
	position(_position)
	, velocity({ 0,0,0 })
	, status(_status)
	, hp(_status.maxHP)
	, alive(true)
	, crouch(false)
	, currentHeight(_status.height)
	, currentEyeHeight(_status.eyeHeight)
	, modelHandle(-1) {

	CollisionManager::GetIns().Register(this);
};		//コンストラクタ

Character::~Character() {
	CollisionManager::GetIns().Unregister(this);
	if (modelHandle != -1) MV1DeleteModel(modelHandle);
}