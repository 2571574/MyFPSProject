#include "BaseProjectile.h"
#include "CollisionManager.h"
#include "EnemyManager.h"
#include "Player.h"
#include "EffectManager.h"

namespace {
	constexpr float FPS_BASE = 60.0f;
	constexpr int HEADSHOT_MULTIPLIER = 2;
}
BaseProjectile::BaseProjectile(VECTOR start, TEAMID _id, const GunStatus& _spec, VECTOR direction)
	: startpos(start)
	, pos(start)
	, dir(direction)
	, id(_id)
	, spec(_spec)
	, alive(true) {
	dir = VNorm(dir);
}
/*弾の更新*/
void BaseProjectile::Update() {
	float move = spec.projectileSpeed * Time::GetIns().GetDelta()* FPS_BASE;		//進む量
	VECTOR nextpos = VAdd(pos, VScale(dir, move));		//次のフレームの時の位置

	HitInfo hit =
		CollisionManager::GetIns().CheckProjectile(pos, nextpos, spec.projectileSize, id);	//弾が敵に当たったか判定

	//当たっていた場合
	if (hit.character != nullptr || hit.isWallHit) {

		VECTOR hitPoint = hit.hitPos;
		bool hitEnemy = false;
		bool isHeadShot = false;

		//当たっていたらその敵の被弾処理
		if (spec.AOE) {
			hitEnemy = Explode(hitPoint);
		}
		else if (hit.character) {
			hitEnemy = true;
			isHeadShot = hit.isHeadShot;
			int lastDamage = hit.isHeadShot ? spec.damage * HEADSHOT_MULTIPLIER : spec.damage;
			if (hit.isHeadShot)Debug::Log("Headshot");
			else Debug::Log("hit");
			hit.character->OnHit(lastDamage, spec.id);
			if (hitEnemy && id == TEAMID::ID_FRIENDLY) {
				if (Player* p = EnemyManager::GetIns().GetPlayer()) {
					p->HitRecord(isHeadShot);
				}
			}
			Debug::Log("EffectSpawn true");
			EffectManager::GetIns().CreateHitEffect(hit.hitPos, hit.hitNormal, true);
		}
		else if (hit.isWallHit) {
			Debug::Log("EffectSpawn false");
			EffectManager::GetIns().CreateHitEffect(hit.hitPos, hit.hitNormal, false);
		}
		alive = false;		//弾の生存タグを消す
	}
	if (alive) {
		//射程以上進んだら弾の生存タグを消す
		if (VSize(VSub(pos, startpos)) > spec.range) {
			if (spec.AOE) Explode(nextpos);
			alive = false;
		}
		//移動処理
		pos = nextpos;
	}
}

/*弾の描画*/
void BaseProjectile::Draw() {
	DrawSphere3D(pos, spec.projectileSize, CIRCLE_DIVNUM, GetColor(0, 0, 0), GetColor(0, 0, 0), true);
}


bool BaseProjectile::Explode(VECTOR hitPos) {
	return CollisionManager::GetIns().ProcessExplotion(hitPos, spec.explodeArea, spec.damage, spec.knockbackP, true, id, spec.id,spec.friendlyFire);
}