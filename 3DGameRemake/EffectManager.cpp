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
	for (const auto& e : effects) {
		e->Draw();
	}
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
	AddEffect(std::make_unique<MuzzleFlashEffect>(pos, dir, 0.1f, size, GetColor(255, 255, 0)));
}