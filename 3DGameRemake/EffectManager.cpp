#include "EffectManager.h"
#include "SoundManager.h"
#include "Param/Visual.h"
#include "Param/Global.h"
#include "Param/System.h"
#include <algorithm>

EffectManager& EffectManager::GetIns() {
	static EffectManager ins;
	return ins;
}

void EffectManager::AddEffect(std::unique_ptr<Effect> effect) {
	if (effect) {
		effects.push_back(std::move(effect));
	}
}

void EffectManager::Update() {
	for (auto& e : effects) {
		e->Update();
	}
	effects.erase(std::remove_if(effects.begin(), effects.end(), [](const std::unique_ptr<Effect>& e) { return !e->IsAlive(); }), effects.end());
}

void EffectManager::Draw() {
	SetWriteZBuffer3D(FALSE);
	for (const auto& e : effects) {
		e->Draw();
	}
	SetWriteZBuffer3D(TRUE);
}

void EffectManager::Clear() {
	effects.clear();
}

void EffectManager::CreateDeathParticle(VECTOR pos, float floorY, ENEMYTYPE type) {
	int color = GetColor(255, 255, 255);

	if (type == ENEMYTYPE::MELEE) color = GetColor(Visual::Effect::COLOR_EFFECT_MELEE.r, Visual::Effect::COLOR_EFFECT_MELEE.g, Visual::Effect::COLOR_EFFECT_MELEE.b);
	else if (type == ENEMYTYPE::RIFLE) color = GetColor(Visual::Effect::COLOR_EFFECT_RIFLE.r, Visual::Effect::COLOR_EFFECT_RIFLE.g, Visual::Effect::COLOR_EFFECT_RIFLE.b);
	else if (type == ENEMYTYPE::SNIPER) color = GetColor(Visual::Effect::COLOR_EFFECT_SNIPER.r, Visual::Effect::COLOR_EFFECT_SNIPER.g, Visual::Effect::COLOR_EFFECT_SNIPER.b);
	else if (type == ENEMYTYPE::ROLLING) color = GetColor(Visual::Effect::COLOR_EFFECT_ROLLING.r, Visual::Effect::COLOR_EFFECT_ROLLING.g, Visual::Effect::COLOR_EFFECT_ROLLING.b);

	int particleCount = Visual::Effect::BASE_PARTICLE_COUNT_DEATH_MIN + GetRand(Visual::Effect::BASE_PARTICLE_COUNT_DEATH_RAND);
	for (int i = 0; i < particleCount; ++i) {
		float theta = (GetRand(Visual::Effect::PI_APPROX_INT) / Visual::Effect::RANDOM_PERCENT_DIVISOR) * 2.0f;
		float phi = acosf(1.0f - 2.0f * (GetRand(Global::Math::PERCENT_MAX) * Global::Math::RATIO_FROM_PERCENT));;

		float speed = (GetRand(Global::Math::PERCENT_MAX) * Global::Math::RATIO_FROM_PERCENT) * Visual::Effect::PARTICLE_SPEED_RANDOM_MULT + Visual::Effect::PARTICLE_SPEED_BASE;;

		VECTOR vel;
		vel.x = sinf(phi) * cosf(theta) * speed;
		vel.y = cosf(phi) * speed;
		vel.z = sinf(phi) * sinf(theta) * speed;
		vel.y += (GetRand(Global::Math::PERCENT_MAX) * Global::Math::RATIO_FROM_PERCENT) * Visual::Effect::DEATH_VEL_Y_RANDOM_MULT + Visual::Effect::DEATH_VEL_Y_BASE;

		float life = Visual::Effect::DEATH_LIFE_BASE + (GetRand(Global::Math::PERCENT_MAX) / Visual::Effect::RANDOM_PERCENT_DIVISOR);
		float size = Visual::Effect::DEATH_SIZE_BASE + (GetRand(Visual::Effect::DEATH_SIZE_RANDOM_MULT) / Visual::Effect::RANDOM_PERCENT_DIVISOR);

		AddEffect(std::make_unique<DeathEffect>(pos, vel, life, size, floorY, color));
	}
}

void EffectManager::CreateMuzzleFlash(VECTOR pos, VECTOR dir, float size) {
	AddEffect(std::make_unique<MuzzleFlashEffect>(pos, dir, Visual::Effect::MUZZLE_FLASH_LIFE, size, GetColor(Visual::Effect::COLOR_MUZZLE_FLASH.r, Visual::Effect::COLOR_MUZZLE_FLASH.g, Visual::Effect::COLOR_MUZZLE_FLASH.b)));
}

void EffectManager::CreateHitEffect(VECTOR pos, VECTOR normal, bool isEnemy) {
	int color = isEnemy ? GetColor(Visual::Effect::COLOR_EFFECT_HIT.r, Visual::Effect::COLOR_EFFECT_HIT.g, Visual::Effect::COLOR_EFFECT_HIT.b) 
						: GetColor(Visual::Effect::COLOR_EFFECT_WHITE.r, Visual::Effect::COLOR_EFFECT_WHITE.g, Visual::Effect::COLOR_EFFECT_WHITE.b);

	int particleCount = isEnemy ? Visual::Effect::HIT_PARTICLE_COUNT_ENEMY : Visual::Effect::HIT_PARTICLE_COUNT_WALL;

	if (VSquareSize(normal) < System::Collision::MIN_DIST_SQUARED) normal = VGet(0, 1, 0);
	normal = VNorm(normal);

	VECTOR up = VGet(0, 1, 0);
	VECTOR right = VCross(up, normal);
	if (VSquareSize(right) < System::Collision::MIN_DIST_SQUARED) {
		right = VGet(1, 0, 0);
	}
	else {
		right = VNorm(right);
	}
	up = VNorm(VCross(normal, right));

	for (int i = 0; i < particleCount; ++i) {
		// 法線を中心とした半球状にランダムなベクトルを生成
		float theta = (GetRand(Visual::Effect::PI_APPROX_INT) / Visual::Effect::RANDOM_PERCENT_DIVISOR) * 2.0f;
		float phi = acosf(1.0f - (GetRand(Global::Math::PERCENT_MAX) / Visual::Effect::RANDOM_PERCENT_DIVISOR));

		float speed = (GetRand(Global::Math::PERCENT_MAX) / Visual::Effect::RANDOM_PERCENT_DIVISOR) * Visual::Effect::HIT_SPEED_RANDOM_MULT + Visual::Effect::HIT_SPEED_BASE;

		float localX = sinf(phi) * cosf(theta);
		float localY = sinf(phi) * sinf(theta);
		float localZ = cosf(phi);

		VECTOR vel;
		vel.x = right.x * localX + up.x * localY + normal.x * localZ;
		vel.y = right.y * localX + up.y * localY + normal.y * localZ;
		vel.z = right.z * localX + up.z * localY + normal.z * localZ;

		vel = VScale(vel, speed);

		float life = Visual::Effect::HIT_LIFE_BASE + (GetRand(Visual::Effect::HIT_LIFE_RANDOM) / Visual::Effect::RANDOM_PERCENT_DIVISOR);
		float size = Visual::Effect::HIT_SIZE_BASE + (GetRand(Visual::Effect::HIT_SIZE_RANDOM) / Visual::Effect::RANDOM_PERCENT_DIVISOR);

		AddEffect(std::make_unique<HitEffect>(pos, vel, life, size, Visual::Effect::EFFECT_FLOOR_Y_LIMIT, color));
	}
}

void EffectManager::CreateSpawnEffect(VECTOR pos, float height, float radius, float life) {
	AddEffect(std::make_unique<SpawnEffect>(pos, height, radius, life));
}

void EffectManager::CreateExplosionEffect(VECTOR pos, float radius, int color) {
	AddEffect(std::make_unique<ExplosionEffect>(pos, radius, color, Visual::Effect::EXPLOSION_LIFE));
	SoundManager::GetIns().Play3DSE("Resource/Sound/explode.ogg", pos, Visual::Effect::EXPLOSION_SOUND_RADIUS);
}

void EffectManager::CreateHitScanTrail(VECTOR start, VECTOR end, int color) {
	AddEffect(std::make_unique<HitScanTrail>(start, end, color, Visual::Effect::TRAIL_HITSCAN_LIFE));
}

void EffectManager::CreateProjectileTrail(VECTOR start, VECTOR end, float radius, int color) {
	AddEffect(std::make_unique<ProjectileTrailEffect>(start, end, radius, color, Visual::Effect::TRAIL_PROJECTILE_LIFE));
}