#pragma once
#include "DxLib.h"
#include "Time.h"
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
	unsigned int color;
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