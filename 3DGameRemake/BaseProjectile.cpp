#include "BaseProjectile.h"
#include "CollisionManager.h"
#include "EnemyManager.h"

/*弾の更新*/
void BaseProjectile::Update() {
	float move = spec.projectileSpeed * Time::GetIns().GetDelta()*60;		//進む量
	VECTOR nextpos = VAdd(pos, VScale(dir, move));		//次のフレームの時の位置

	HitInfo hit =
		CollisionManager::GetIns().CheckProjectile(pos, nextpos, spec.projectileSize, id);	//弾が敵に当たったか判定

	//地面に当たったか判定
	bool hitGround = (nextpos.y <= 0.0f);

	//当たっていた場合
	if (hit.character != nullptr || hitGround) {
		//着弾点の計算
		VECTOR hitPoint = hit.character ? hit.character->GetPos() : nextpos;

		//当たっていたらその敵の被弾処理
		if (spec.AOE) {
			Explode(hitPoint);
		}
		else if (hit.character) {
			int lastDamage = hit.isHeadShot ? spec.damage * 2 : spec.damage;
			if (hit.isHeadShot)Debug::Log("Headshot");
			else Debug::Log("hit");
			hit.character->OnHit(lastDamage);
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
		pos.x += dir.x * move;
		pos.y += dir.y * move;
		pos.z += dir.z * move;
	}
}

/*弾の描画*/
void BaseProjectile::Draw() {
	DrawSphere3D(pos, spec.projectileSize, CIRCLE_DIVNUM, GetColor(0, 0, 0), GetColor(0, 0, 0), true);
}


void BaseProjectile::Explode(VECTOR hitPos) {
	CollisionManager::GetIns().ProcessExplotion(hitPos, spec.explodeArea, spec.damage, id);
	DrawSphere3D(hitPos, spec.explodeArea, CIRCLE_DIVNUM, GetColor(255, 150, 0), GetColor(255, 150, 0), FALSE);
}