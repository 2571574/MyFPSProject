#include "RollingEnemy.h"
#include "Player.h"
#include "EnemyManager.h"

RollingEnemy::RollingEnemy(VECTOR pos, Player* target)
	:Enemy(pos, CHARA_STATUS::ROLL_ENEMY, target)
	, isExploding(false)
	, explodeTimer(2.0f)
	, triggerDist(5.0f) {
	explodeSpec = ENEMY_GUN::DESTRUCT;
}

void RollingEnemy::Update() {
	if (hp <= 0) {
		alive = false; 
		return; 
	}

	float dt = Time::GetIns().GetDelta();
	float distToPlayer = VSize(VSub(target->GetPos(), position));

	if (!isExploding) {
		VECTOR moveTarget = UpdateNavigation(target, dt);
		VECTOR dir = VNorm(VSub(moveTarget, position));
		dir.y = 0.0f;

		ApplyMovement(dir, dt);

		if (distToPlayer < triggerDist) {
			isExploding = true;
		}
	}
	else {
		ApplyMovement(VGet(0, 0, 0), dt);
		explodeTimer -= dt;

		if (explodeTimer <= 0.0f) {
			Action();
		}
	}
}

void RollingEnemy::Action() {
	float radius = explodeSpec.explodeArea;

	float distToPlayer = VSize(VSub(target->GetPos(), position));

	if (distToPlayer <= radius) {
		float damageRate = 1.0f - (distToPlayer / radius);
		target->TakeDamage((int)(explodeSpec.damage) * damageRate);
	}

	auto& enemies = EnemyManager::GetIns().GetEnemies();

	for(auto&enemy : enemies){
		if (!enemy || !enemy->IsAlive() || enemy.get() == this) continue;

		float dist = VSize(VSub(enemy->GetPos(), position));
		if (dist <= radius) {
			enemy->OnHit(explodeSpec.damage);
		}
	}

	alive = false;
}

void RollingEnemy::Draw() {
	int color = GetColor(255, 165, 0);
	if (isExploding) {
		if ((int)(explodeTimer * 10) % 2 == 0)color = GetColor(255, 0, 0);
	}

	DrawSphere3D(VAdd(position, VGet(0.0f, status.height, 0.0f)), status.width, 16, color, color, TRUE);
}
