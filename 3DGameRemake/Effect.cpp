#include "Effect.h"
#include "Parameter.h"
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
	, size(s)
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

SpawnEffect::SpawnEffect(VECTOR pos, float height, float radius, float life)
	: Effect(pos,life)
	, targetHeight(height)
	, targetRadius(radius){}

void SpawnEffect::Update() {
	float dt = Time::GetIns().GetDelta();
	float dt60 = dt * 60.0f;

	lifeTime -= dt;
	if (lifeTime < 0.0f && particles.empty()) {
		alive = false;
		return;
	}

	if (lifeTime >= 0.0f) {
		float progress = 1.0f - (lifeTime / maxLifeTime);
		float scanY = 0.0f;

		if (progress < 0.5f) {
			scanY = (progress * 2.0f) * targetHeight;
		}
		else {
			scanY = ((1.0f - progress) * 2.0f) * targetHeight;
		}

		VECTOR center = VAdd(position, VGet(0, scanY, 0));

		int spawnCount = 4;
		for (int i = 0; i < spawnCount; i++) {
			float angle = (GetRand(314) / 100.0f) * 2.0f;
			float dist = targetRadius + (GetRand(10) / 100.0f);
			VECTOR pPos = VAdd(center, VGet(cosf(angle) * dist, (GetRand(20) - 10) / 100.0f, sinf(angle) * dist));

			VECTOR pVel = VGet((GetRand(20) - 10) / 500.0f, (GetRand(20) - 5) / 200.0f, (GetRand(20) - 10) / 500.0f);

			float pLife = 0.2f + (GetRand(20) / 100.0f);
			float pSize = 0.03f + (GetRand(3) / 100.0f);

			int pColor = (GetRand(1) == 0) ? GetColor(255, 255, 255) : GetColor(200, 100, 255);

			particles.push_back({ pPos, pVel, pLife, pLife, pSize,pColor });
		}
	}

	for (auto it = particles.begin(); it != particles.end();) {
		it->life -= dt;
		if (it->life <= 0.0f) {
			it = particles.erase(it);
		}
		else {
			it->pos = VAdd(it->pos, VScale(it->vel, dt60));
			it->vel.x *= std::pow(0.9f, dt60);
			it->vel.z *= std::pow(0.9f, dt60);
			++it;
		}
	}
}

void SpawnEffect::Draw() {
	for (const auto& p : particles) {
		float scale = p.life / p.maxLife;
		float currentSize = p.size * scale;
		if (currentSize <= 0.0f) continue;

		int alpha = static_cast<int>(255 * scale);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);

		VECTOR pos1 = VAdd(p.pos, VGet(-currentSize, -currentSize, -currentSize));
		VECTOR pos2 = VAdd(p.pos, VGet(currentSize, currentSize, currentSize));

		DrawCube3D(pos1, pos2, p.color, p.color, TRUE);
	}

	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

ExplosionEffect::ExplosionEffect(VECTOR pos, float radius, int c, float life)
	: Effect(pos, life)
	, maxRadius(radius)
	, color(c) { 
	Debris d;
	d.pos = pos;
	int debrisCount = 15 + GetRand(10);
	for (int i = 0; i < debrisCount; ++i) {
		float theta = (GetRand(314) / 100.0f) * 2.0f;
		float phi = acosf(1.0f - 2.0f * (GetRand(100) / 100.0f));
		float speed = (GetRand(100) / 100.0f) * 0.8f + 0.2f;

		d.vel.x = sinf(phi) * cosf(theta) * speed;
		d.vel.y = cosf(phi) * speed;
		d.vel.z = sinf(phi) * sinf(theta) * speed;

		d.rotAxis = VNorm(VGet(GetRand(100) - 50.0f, GetRand(100) - 50.0f, GetRand(100) - 50.0f));
		if (VSize(d.rotAxis) < 0.01f)d.rotAxis = VGet(0, 1, 0);
		d.angle = 0.0f;
		d.rotSpeed = (GetRand(100) / 100.0f) * 0.5f + 0.1f;
		d.size = 0.04f + (GetRand(6) / 100.0f);

		debrisList.push_back(d);
	}
}

void ExplosionEffect::Update() {
	float dt = Time::GetIns().GetDelta();
	float dt60 = dt * 60.0f;

	lifeTime -= dt;
	if (lifeTime <= 0.0f) {
		alive = false;
		return;
	}

	for (auto& d : debrisList) {
		d.vel.y -= 0.025f * dt60;
		d.pos = VAdd(d.pos, VScale(d.vel, dt60));

		if (d.pos.y - d.size < -10.0f) {
			d.pos.y = -10.0f + d.size;
			d.vel.y *= -0.5f;
			d.vel.x *= std::pow(0.7f, dt60);
			d.vel.z *= std::pow(0.7f, dt60);
		}
		else {
			float friction = std::pow(0.98f, dt60);
			d.vel.x *= friction;
			d.vel.z *= friction;
		}
		d.angle += d.rotSpeed * dt60;
	}
}

void ExplosionEffect::Draw() {
	float scale = lifeTime / maxLifeTime;
	if (scale < 0.0f)scale = 0.0f;

	float coreProgress = 1.0f - scale;
	float coreScale = 1.0f - std::pow(1.0f - coreProgress, 3.0f);
	float currentRadius = maxRadius * coreScale;

	int coreAlpha = static_cast<int>(150 * scale);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, coreAlpha);
	DrawSphere3D(position, currentRadius, CIRCLE_DIVNUM, color, color, TRUE);

	int debrisAlpha = static_cast<int>(255 * (scale * scale));
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, debrisAlpha);

	for (const auto& d : debrisList) {
		MATRIX rot = MGetRotAxis(d.rotAxis, d.angle);
		VECTOR topOffset = VTransform(VGet(0, d.size, 0), rot);
		VECTOR bottomOffset = VTransform(VGet(0, -d.size, 0), rot);

		VECTOR top = VAdd(d.pos, topOffset);
		VECTOR bottom = VAdd(d.pos, bottomOffset);

		DrawCone3D(top, bottom, d.size, 3, color, color, TRUE);
	}

	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}


HitScanTrail::HitScanTrail(VECTOR start, VECTOR end, int c, float life)
	: Effect(start, life)
	, startPos(start)
	, endPos(end)
	, color(c)
	, initialRad(0.03f) {}

void HitScanTrail::Update() {
	float dt = Time::GetIns().GetDelta();
	lifeTime -= dt;
	if (lifeTime <= 0.0f) {
		alive = false;
	}
}

void HitScanTrail::Draw() {
	float scale = lifeTime / maxLifeTime;
	if (scale < 0.0f)scale = 0.0f;

	float easeScale = scale * scale;
	VECTOR currentStart = VAdd(endPos, VScale(VSub(startPos, endPos), scale));
	float currentRad = initialRad * scale;

	int alpha = static_cast<int>(255 * sqrtf(scale));

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
	DrawCapsule3D(currentStart, endPos, currentRad, 8, color, color, TRUE);

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
	DrawCapsule3D(currentStart, endPos, currentRad * 0.3f, 8, color, color, TRUE);

	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

ProjectileTrailEffect::ProjectileTrailEffect(VECTOR start, VECTOR end, float radius, int c, float life)
	: Effect(start, life)
	, startPos(start)
	, endPos(end)
	, color(c)
	, initialRadius(radius) {}

void ProjectileTrailEffect::Update() {
	float dt = Time::GetIns().GetDelta();
	lifeTime -= dt;
	if (lifeTime <= 0.0f) {
		alive = false;
	}
}

void ProjectileTrailEffect::Draw() {
	float scale = lifeTime / maxLifeTime;
	if (scale < 0.0f) scale = 0.0f;

	float currentRadius = initialRadius * scale;

	int alpha = static_cast<int>(255 * scale);

	SetWriteZBuffer3D(FALSE);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);

	DrawCapsule3D(startPos, endPos, currentRadius, 6, color, color, TRUE);

	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	SetWriteZBuffer3D(TRUE);
}