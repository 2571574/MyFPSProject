#pragma once
#include "DxLib.h"
#include "Time.h"

#include <vector>

class Effect
{
protected:
	VECTOR position;
	float lifeTime;
	float maxLifeTime;
	bool alive;

public:
	Effect(VECTOR pos, float life) : position(pos), lifeTime(life), maxLifeTime(life), alive(true) {}
	virtual ~Effect() = default;

	virtual void Update() = 0;
	virtual void Draw() = 0;
	bool IsAlive() const { return alive; }
};

class DeathEffect : public Effect {
private:
	VECTOR velocity;
	float size;
	int color;
	float floorY;

	VECTOR rotAxis;
	float angle;
	float rotSpeed;

public:
	DeathEffect(VECTOR pos, VECTOR vel, float life, float s, float fY, int c);
	void Update() override;
	void Draw() override;
};

class MuzzleFlashEffect : public Effect {
private:
	VECTOR direction;
	float size;
	int color;
public:
	MuzzleFlashEffect(VECTOR pos, VECTOR dir, float life, float s, int c);
	void Update() override;
	void Draw() override;
};

class HitEffect : public Effect {
private:
	VECTOR velocity;
	float size;
	int color;
	float floorY;
public:
	HitEffect(VECTOR pos, VECTOR vel, float life, float s, float fY, int c);
	void Update() override;
	void Draw() override;
};

class SpawnEffect : public Effect {
private:
	float targetHeight;
	float targetRadius;
	struct Particle {
		VECTOR pos;
		VECTOR vel;
		float life;
		float maxLife;
		float size;
		int color;
	};
	std::vector<Particle>particles;
public:
	SpawnEffect(VECTOR pos, float height, float radius, float life);
	void Update() override;
	void Draw() override;
};

class ExplosionEffect : public Effect {
private:
	float maxRadius;
	int color;

	struct Debris {
		VECTOR pos;
		VECTOR vel;
		VECTOR rotAxis;
		float angle;
		float rotSpeed;
		float size;
	};
	std::vector<Debris> debrisList;
public:
	ExplosionEffect(VECTOR pos, float radius, int c, float life);
	void Update() override;
	void Draw() override;
};

class HitScanTrail : public Effect {
private:
	VECTOR startPos;
	VECTOR endPos;
	int color;
	float initialRad;
public:
	HitScanTrail(VECTOR start, VECTOR end, int c, float life);
	void Update() override;
	void Draw() override;
};

class ProjectileTrailEffect : public Effect {
private:
	VECTOR startPos;
	VECTOR endPos;
	int color;
	float initialRadius;

public:
	ProjectileTrailEffect(VECTOR start, VECTOR end, float radius, int c, float life);
	void Update() override;
	void Draw() override;
};