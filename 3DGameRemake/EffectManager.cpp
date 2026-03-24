#include "EffectManager.h"
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

	if (type == ENEMYTYPE::MELEE) color = GetColor(255, 255, 0);
	else if (type == ENEMYTYPE::RIFLE) color = GetColor(0, 0, 255);
	else if (type == ENEMYTYPE::SNIPER) color = GetColor(0, 255, 0);
	else if (type == ENEMYTYPE::ROLLING) color = GetColor(255, 165, 0);

	int particleCount = 15 + GetRand(5);
	for (int i = 0; i < particleCount; ++i) {
		float theta = (GetRand(314) / 100.0f) * 2.0f;
		float phi = acosf(1.0f - 2.0f * (GetRand(100) / 100.0f));

		float speed = (GetRand(100) / 100.0f) * 0.4f + 0.1f; 

		VECTOR vel;
		vel.x = sinf(phi) * cosf(theta) * speed;
		vel.y = cosf(phi) * speed;
		vel.z = sinf(phi) * sinf(theta) * speed;
		vel.y += (GetRand(100) / 100.0f) * 0.3f + 0.2f;

		float life = 1.0f + (GetRand(100) / 100.0f); 
		float size = 0.05f + (GetRand(10) / 100.0f);

		AddEffect(std::make_unique<DeathEffect>(pos, vel, life, size, floorY, color));
	}
}

void EffectManager::CreateMuzzleFlash(VECTOR pos, VECTOR dir, float size) {
	AddEffect(std::make_unique<MuzzleFlashEffect>(pos, dir, 0.05f, size, GetColor(255, 255, 0)));
}

void EffectManager::CreateHitEffect(VECTOR pos, VECTOR normal, bool isEnemy) {
	int color = isEnemy ? GetColor(255, 100, 0) : GetColor(255,255,255);
	int particleCount = isEnemy ? 12 : 6;

	if (VSquareSize(normal) < 0.01f)normal = VGet(0, 1, 0);
	normal = VNorm(normal);

	VECTOR up = VGet(0, 1, 0);
	VECTOR right = VCross(up, normal);
	if (VSquareSize(right) < 0.01f) {
		right = VGet(1, 0, 0);
	}
	else {
		right = VNorm(right);
	}
	up = VNorm(VCross(normal, right));

	for (int i = 0; i < particleCount; ++i) {
		// 法線を中心とした半球状にランダムなベクトルを生成
		float theta = (GetRand(314) / 100.0f) * 2.0f;
		float phi = acosf(1.0f - (GetRand(100) / 100.0f));

		float speed = (GetRand(100) / 100.0f) * 0.25f + 0.1f;

		float localX = sinf(phi) * cosf(theta);
		float localY = sinf(phi) * sinf(theta);
		float localZ = cosf(phi);

		VECTOR vel;
		vel.x = right.x * localX + up.x * localY + normal.x * localZ;
		vel.y = right.y * localX + up.y * localY + normal.y * localZ;
		vel.z = right.z * localX + up.z * localY + normal.z * localZ;

		vel = VScale(vel, speed);

		float life = 0.4f + (GetRand(40) / 100.0f);
		float size = 0.04f + (GetRand(3) / 100.0f);

		// 床の高さはステージ下限として一旦 -10.0f 固定（空中で当たっても奈落に落ちて消える）
		AddEffect(std::make_unique<HitEffect>(pos, vel, life, size, -10.0f, color));
	}
}

void EffectManager::CreateSpawnEffect(VECTOR pos, float height, float radius, float life) {
	AddEffect(std::make_unique<SpawnEffect>(pos, height, radius, life));
}

void EffectManager::CreateExplosionEffect(VECTOR pos, float radius, int color) {
	AddEffect(std::make_unique<ExplosionEffect>(pos, radius, color, 0.6f));
}

void EffectManager::CreateHitScanTrail(VECTOR start, VECTOR end, int color) {
	AddEffect(std::make_unique<HitScanTrail>(start, end, color, 0.25f));
}

void EffectManager::CreateProjectileTrail(VECTOR start, VECTOR end, float radius, int color) {
	AddEffect(std::make_unique<ProjectileTrailEffect>(start, end, radius, color, 0.12f));
}