#pragma once
#include <vector>
#include <memory>
#include "Effect.h"
#include "Status.h"

class EffectManager
{
private:
	std::vector<std::unique_ptr<Effect>> effects;
	EffectManager() = default;

public:
	static EffectManager& GetIns();

	void AddEffect(std::unique_ptr<Effect> effect);
	void Update();
	void Draw();
	void Clear();

	void CreateDeathParticle(VECTOR pos, float floorY, ENEMYTYPE type);
	void CreateMuzzleFlash(VECTOR pos, VECTOR dir, float size);
	void CreateHitEffect(VECTOR pos, VECTOR normal, bool isEnemy);
};

