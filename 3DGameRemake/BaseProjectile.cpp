#include "BaseProjectile.h"
#include "EnemyManager.h"

/*弾の更新*/
void BaseProjectile::Update() {
	float move = spec.projectileSpeed * Time::GetIns().GetDelta()*60;		//進む量
	VECTOR nextpos = VAdd(pos, VScale(dir, move));		//次のフレームの時の位置

	Enemy* hitEnemy =
		EnemyManager::GetIns().CheckProjectile(pos, nextpos, spec.projectileSize, id);	//弾が敵に当たったか判定

	//地面に当たったか判定
	bool hitGround = (nextpos.y <= 0.0f);

	//当たっていた場合
	if (hitEnemy != nullptr || hitGround) {
		//着弾点の計算
		VECTOR hitPoint = hitEnemy ? hitEnemy->GetPos() : nextpos;

		//当たっていたらその敵の被弾処理
		if (spec.AOE) {
			Explode(hitPoint);
		}
		else if (hitEnemy) {
			hitEnemy->OnHit(spec.damage);
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
	float radius = (float)spec.explodeArea;
	if (radius <= 0.0f)return;
	auto& enemies = EnemyManager::GetIns().GetEnemies();
	for (auto& enemy : enemies) {
		if (!enemy || !enemy->IsAlive()) continue;
		
		if (enemy->GetID() == id)continue;

		float dist = VSize(VSub(enemy->GetPos(), hitPos));
		if (dist <= radius) {
			float damageRate = 1.0f - (dist / radius);
			enemy->OnHit(spec.damage * damageRate);
		}
	}
	DrawSphere3D(hitPos, radius, CIRCLE_DIVNUM, GetColor(255, 150, 0), GetColor(255, 150, 0), FALSE);
}