#include "BaseProjectile.h"
#include "CollisionManager.h"
#include "EnemyManager.h"
#include "Player.h"
#include "EffectManager.h"
#include "Param/Global.h"
#include "Param/Item.h"
#include "Param/Visual.h"

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
	float move = spec.projectileSpeed * Time::GetIns().GetDelta() * Global::Math::FPS_BASE;
	VECTOR nextpos = VAdd(pos, VScale(dir, move));

	HitInfo hit =
		CollisionManager::GetIns().CheckProjectile(pos, nextpos, spec.projectileSize, id);

	VECTOR actualNextPos = nextpos;

	//当たっていた場合
	if (hit.character != nullptr || hit.isWallHit) {

		VECTOR hitPoint = hit.hitPos;
		actualNextPos = hitPoint;

		bool hitEnemy = false;
		bool isHeadShot = false;

		//当たっていたらその敵の被弾処理
		if (spec.AOE) {
			hitEnemy = Explode(hitPoint);
			int color = GetColor(Visual::Effect::COLOR_EXPLOSION.r, Visual::Effect::COLOR_EXPLOSION.g, Visual::Effect::COLOR_EXPLOSION.b);
			EffectManager::GetIns().CreateExplosionEffect(hitPoint, spec.explodeArea, color);
			if (hitEnemy && id == TEAMID::ID_FRIENDLY) {
				if (Player* p = EnemyManager::GetIns().GetPlayer()) {
					p->HitRecord(false, false);
				}
			}
		}
		else if (hit.character) {
			hitEnemy = true;
			isHeadShot = hit.isHeadShot;
			int lastDamage = hit.isHeadShot ? spec.damage * Item::Weapon::HEADSHOT_MULTIPLIER : spec.damage;
			if (hit.isHeadShot)Debug::Log("Headshot");
			else Debug::Log("hit");
			hit.character->OnHit(lastDamage, spec.id);
			bool isKill = (hit.character->GetHP() <= 0);

			if (hitEnemy && id == TEAMID::ID_FRIENDLY) {
				if (Player* p = EnemyManager::GetIns().GetPlayer()) {
					p->HitRecord(isHeadShot, isKill);
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

	// 味方の弾はライトグレー、敵の弾はオレンジで色分け
	int trailColor = (id == TEAMID::ID_FRIENDLY) ?
		GetColor(Visual::Effect::COLOR_TRAIL_FRIENDLY.r, Visual::Effect::COLOR_TRAIL_FRIENDLY.g, Visual::Effect::COLOR_TRAIL_FRIENDLY.b) :
		GetColor(Visual::Effect::COLOR_TRAIL_ENEMY.r, Visual::Effect::COLOR_TRAIL_ENEMY.g, Visual::Effect::COLOR_TRAIL_ENEMY.b);

	if (spec.id == WeaponID::LR) trailColor = GetColor(Visual::Effect::COLOR_TRAIL_LAUNCHER.r, Visual::Effect::COLOR_TRAIL_LAUNCHER.g, Visual::Effect::COLOR_TRAIL_LAUNCHER.b);
	EffectManager::GetIns().CreateProjectileTrail(pos, actualNextPos, spec.projectileSize * Item::Projectile::TRAIL_RADIUS_MULTIPLIER, trailColor);

	if (alive) {
		//射程以上進んだら弾の生存タグを消す
		if (VSize(VSub(actualNextPos, startpos)) > spec.range) {
			if (spec.AOE) {
				Explode(actualNextPos);
				int color = GetColor(Visual::Effect::COLOR_EXPLOSION.r, Visual::Effect::COLOR_EXPLOSION.g, Visual::Effect::COLOR_EXPLOSION.b);
				EffectManager::GetIns().CreateExplosionEffect(actualNextPos, spec.explodeArea, color);
			}
			alive = false;
		}
		//移動処理
		pos = actualNextPos;
	}
}

/*弾の描画*/
void BaseProjectile::Draw() {}

bool BaseProjectile::Explode(VECTOR hitPos) {
	return CollisionManager::GetIns().ProcessExplosion(hitPos, spec.explodeArea, spec.damage, spec.knockbackP, true, id, spec.id, spec.friendlyFire);
}