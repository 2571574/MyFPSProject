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
	float move = spec.projectileSpeed * Time::GetIns().GetDelta() * FPS_BASE;		//進む量
	VECTOR nextpos = VAdd(pos, VScale(dir, move));		//次のフレームの時の位置

	HitInfo hit =
		CollisionManager::GetIns().CheckProjectile(pos, nextpos, spec.projectileSize, id);	//弾が敵に当たったか判定

	VECTOR actualNextPos = nextpos; // 軌跡を引くための「実際の到達点」

	//当たっていた場合
	if (hit.character != nullptr || hit.isWallHit) {

		VECTOR hitPoint = hit.hitPos;
		actualNextPos = hitPoint; // ★修正: 壁の表面や敵の表面を最終到達点にする（突き抜けない）

		bool hitEnemy = false;
		bool isHeadShot = false;

		//当たっていたらその敵の被弾処理
		if (spec.AOE) {
			hitEnemy = Explode(hitPoint);
			int color = GetColor(255, 100, 0);
			EffectManager::GetIns().CreateExplosionEffect(hitPoint, spec.explodeArea, color);
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

	// ★追加: 1フレーム分の移動距離を繋ぐカプセル（リボンの1節）を生成
	// 味方の弾はライトグレー、敵の弾はオレンジで色分け
	int trailColor = (id == TEAMID::ID_FRIENDLY) ? GetColor(180, 180, 180) : GetColor(255, 0, 0);
	if (spec.id == WeaponID::LR) trailColor = GetColor(255, 100, 0);
	EffectManager::GetIns().CreateProjectileTrail(pos, actualNextPos, spec.projectileSize * 1.0f, trailColor);

	if (alive) {
		//射程以上進んだら弾の生存タグを消す
		if (VSize(VSub(actualNextPos, startpos)) > spec.range) {
			if (spec.AOE) {
				Explode(actualNextPos);
				int color = GetColor(255, 100, 0);
				EffectManager::GetIns().CreateExplosionEffect(actualNextPos, spec.explodeArea, color);
			}
			alive = false;
		}
		//移動処理
		pos = actualNextPos; // ★修正: actualNextPosを使用する
	}
}

/*弾の描画*/
void BaseProjectile::Draw() {
}


bool BaseProjectile::Explode(VECTOR hitPos) {
	return CollisionManager::GetIns().ProcessExplosion(hitPos, spec.explodeArea, spec.damage, spec.knockbackP, true, id, spec.id,spec.friendlyFire);
}