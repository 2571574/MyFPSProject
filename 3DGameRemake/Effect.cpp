#include "Effect.h"
#include <cmath>

DeathEffect::DeathEffect(VECTOR pos, VECTOR vel, float life, float s, float fY, int c)
	: Effect(pos, life)
	, velocity(vel)
	, size(s)
	, floorY(fY)
	, color(c){
	rotAxis = VNorm(VGet(GetRand(100) - 50.0f, GetRand(100) - 50.0f, GetRand(100) - 50.0f));
	if (VSize(rotAxis) < 0.01f) rotAxis = VGet(0, 1, 0);
	rotSpeed = (GetRand(100) / 100.0f) * 0.3f + 0.1f;
}

void DeathEffect::Update() {
	float dt = Time::GetIns().GetDelta();
	float dt60 = dt * 60.0f;

	lifeTime -= dt;
	if (lifeTime <= 0.0f) {
		alive = false;
		return;
	}

	velocity.y -= 0.025f * dt60;
	position = VAdd(position, VScale(velocity, dt60));

	
	if (position.y - size < floorY) {
		position.y = floorY + size;
		velocity.y *= -0.5f;

		velocity.x *= std::pow(0.7f, dt60);
		velocity.z *= std::pow(0.7f, dt60);
		rotSpeed *= std::pow(0.8f, dt60);

		if (std::abs(velocity.y) < 0.05f) {
			velocity.y = 0.0f;
		}
	}
	else {
		float friction = std::pow(0.98f, dt60);
		velocity.x *= friction;
		velocity.z *= friction;
	}

	angle += rotSpeed * dt60;
}

void DeathEffect::Draw() {
	float scale = lifeTime / maxLifeTime;
	if (scale < 0.0f) scale = 0.0f;

	int alpha = static_cast<int>(255 * (scale * scale));
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);

	MATRIX rotMat = MGetRotAxis(rotAxis, angle);
	VECTOR topOffset = VTransform(VGet(0, size, 0), rotMat);
	VECTOR bottomOffset = VTransform(VGet(0, -size, 0), rotMat);

	VECTOR top = VAdd(position, topOffset);
	VECTOR bottom = VAdd(position, bottomOffset);

	DrawCone3D(top, bottom, size, 3, color, color, TRUE);

	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}


MuzzleFlashEffect::MuzzleFlashEffect(VECTOR pos, VECTOR dir, float life, float s, int c)
	: Effect(pos, life)
	, direction(VNorm(dir))
	, size(s)
	, color(c) {}

void MuzzleFlashEffect::Update() {
	float dt = Time::GetIns().GetDelta();
	lifeTime -= dt;
	if (lifeTime <= 0.0f) {
		alive = false;
	}
}

void MuzzleFlashEffect::Draw() {
	float scale = lifeTime / maxLifeTime;
	if (scale < 0.0f) scale = 0.0f;

	int alpha = static_cast<int>(255 * scale);
	SetDrawBlendMode(DX_BLENDMODE_ADD, alpha);

	
	VECTOR top = VAdd(position, VScale(direction, size * 4.0f * scale));
	VECTOR bottom = VAdd(position, VScale(direction, -size * 0.5f));
	DrawCone3D(top, bottom, size * scale, 3, color, color, TRUE);

	
	DrawSphere3D(position, size * 0.6f * scale, 8, GetColor(255, 255, 255), GetColor(255, 255, 255), TRUE);

	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

HitEffect::HitEffect(VECTOR pos, VECTOR vel, float life, float s, float fY, int c)
	: Effect(pos, life)
	, velocity(vel)
	, size(c)
	, floorY(fY)
	, color(c) {}


void HitEffect::Update() {
	float dt = Time::GetIns().GetDelta();
	float dt60 = dt * 60.0f;

	lifeTime -= dt;
	if (lifeTime <= 0.0f) {
		alive = false;
		return;
	}

	velocity.y -= 0.025f * dt60;
	position = VAdd(position, VScale(velocity, dt60));

	if (position.y - size < floorY) {
		position.y = floorY + size;
		velocity.y *= -0.5f;

		velocity.x *= std::pow(0.7f, dt60);
		velocity.z *= std::pow(0.7f, dt60);

		if (std::abs(velocity.y) < 0.05f) {
			velocity.y = 0.0f;
		}
	}
	else {
		float friction = std::pow(0.98f, dt60);
		velocity.x *= friction;
		velocity.z *= friction;
	}
}


void HitEffect::Draw() {
	float scale = lifeTime / maxLifeTime;
	if (scale < 0.0f)scale = 0.0f;

	float currentSize = size * scale;

	int alpha = static_cast<int>(255 * (scale * scale));
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);

	VECTOR pos1 = VAdd(position, VGet(-currentSize, -currentSize, -currentSize));
	VECTOR pos2 = VAdd(position, VGet(currentSize, currentSize, currentSize));

	DrawCube3D(pos1, pos2, color, color, TRUE);

	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}