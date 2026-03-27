#include "Effect.h"
#include "Param/Visual.h"
#include "Param/Global.h"
#include "Param/System.h"

#include <cmath>

DeathEffect::DeathEffect(VECTOR pos, VECTOR vel, float life, float s, float fY, int c)
	: Effect(pos, life)
	, velocity(vel)
	, size(s)
	, floorY(fY)
	, color(c) {
	rotAxis = VNorm(VGet(GetRand(Visual::Effect::ROT_AXIS_RAND_RANGE) - Visual::Effect::ROT_AXIS_RAND_OFFSET, GetRand(Visual::Effect::ROT_AXIS_RAND_RANGE) - Visual::Effect::ROT_AXIS_RAND_OFFSET, GetRand(Visual::Effect::ROT_AXIS_RAND_RANGE) - Visual::Effect::ROT_AXIS_RAND_OFFSET));
	if (VSize(rotAxis) < System::Collision::MIN_DIST_SQUARED) rotAxis = VGet(0.0f, 1.0f, 0.0f);
	rotSpeed = (GetRand(100) / 100.0f) * Visual::Effect::PARTICLE_SPEED_RANDOM_MULT + Visual::Effect::PARTICLE_SPEED_BASE;
}

void DeathEffect::Update() {
	float dt = Time::GetIns().GetDelta();
	float dt60 = dt * Global::Math::FPS_BASE;

	lifeTime -= dt;
	if (lifeTime <= 0.0f) {
		alive = false;
		return;
	}

	velocity.y -= Visual::Effect::EFFECT_GRAVITY * dt60;
	position = VAdd(position, VScale(velocity, dt60));

	if (position.y - size < floorY) {
		position.y = floorY + size;
		velocity.y *= Visual::Effect::PARTICLE_BOUNCE_COEFFICIENT;

		velocity.x *= std::pow(Visual::Effect::PARTICLE_FRICTION_GROUND_XZ, dt60);
		velocity.z *= std::pow(Visual::Effect::PARTICLE_FRICTION_GROUND_XZ, dt60);
		rotSpeed *= std::pow(Visual::Effect::PARTICLE_ROT_DECAY, dt60);

		if (std::abs(velocity.y) < Visual::Effect::STOP_VEL_Y_THRESHOLD) {
			velocity.y = 0.0f;
		}
	}
	else {
		float friction = std::pow(Visual::Effect::PARTICLE_FRICTION_AIR_XZ, dt60);
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
	VECTOR topOffset = VTransform(VGet(0.0f, size, 0.0f), rotMat);
	VECTOR bottomOffset = VTransform(VGet(0.0f, -size, 0.0f), rotMat);

	VECTOR top = VAdd(position, topOffset);
	VECTOR bottom = VAdd(position, bottomOffset);

	DrawCone3D(top, bottom, size, Visual::Effect::CONE_SEGMENTS_LOW, color, color, TRUE);

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

	VECTOR top = VAdd(position, VScale(direction, size * Visual::Effect::MUZZLE_FLASH_TOP_SCALE * scale));
	VECTOR bottom = VAdd(position, VScale(direction, -size * Visual::Effect::MUZZLE_FLASH_BOTTOM_SCALE));
	DrawCone3D(top, bottom, size * scale, Visual::Effect::CONE_SEGMENTS_LOW, color, color, TRUE);

	DrawSphere3D(position, size * Visual::Effect::MUZZLE_FLASH_SPHERE_SCALE * scale, Visual::Effect::SPHERE_SEGMENTS_LOW, GetColor(Global::Palette::WHITE.r, Global::Palette::WHITE.g, Global::Palette::WHITE.b), GetColor(Global::Palette::WHITE.r, Global::Palette::WHITE.g, Global::Palette::WHITE.b), TRUE);

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
	float dt60 = dt * Global::Math::FPS_BASE;

	lifeTime -= dt;
	if (lifeTime <= 0.0f) {
		alive = false;
		return;
	}

	velocity.y -= Visual::Effect::EFFECT_GRAVITY * dt60;
	position = VAdd(position, VScale(velocity, dt60));

	if (position.y - size < floorY) {
		position.y = floorY + size;
		velocity.y *= Visual::Effect::PARTICLE_BOUNCE_COEFFICIENT;

		velocity.x *= std::pow(Visual::Effect::PARTICLE_FRICTION_GROUND_XZ, dt60);
		velocity.z *= std::pow(Visual::Effect::PARTICLE_FRICTION_GROUND_XZ, dt60);

		if (std::abs(velocity.y) < Visual::Effect::STOP_VEL_Y_THRESHOLD) {
			velocity.y = 0.0f;
		}
	}
	else {
		float friction = std::pow(Visual::Effect::PARTICLE_FRICTION_AIR_XZ, dt60);
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
	: Effect(pos, life)
	, targetHeight(height)
	, targetRadius(radius) {}

void SpawnEffect::Update() {
	float dt = Time::GetIns().GetDelta();
	float dt60 = dt * Global::Math::FPS_BASE;

	lifeTime -= dt;
	if (lifeTime < 0.0f && particles.empty()) {
		alive = false;
		return;
	}

	if (lifeTime >= 0.0f) {
		float progress = 1.0f - (lifeTime / maxLifeTime);
		float scanY = 0.0f;

		if (progress < Visual::Effect::SPAWN_PROGRESS_HALF) {
			scanY = (progress * Visual::Effect::SPAWN_SCAN_Y_MULT) * targetHeight;
		}
		else {
			scanY = ((1.0f - progress) * Visual::Effect::SPAWN_SCAN_Y_MULT) * targetHeight;
		}

		VECTOR center = VAdd(position, VGet(0.0f, scanY, 0.0f));

		for (int i = 0; i < Visual::Effect::SPAWN_PARTICLE_COUNT; i++) {
			float angle = (GetRand(100) / 100.0f) * DX_PI_F * 2.0f;
			float dist = targetRadius + (GetRand(Visual::Effect::SPAWN_RAD_RAND_RANGE) / 100.0f);
			VECTOR pPos = VAdd(center, VGet(cosf(angle) * dist, (GetRand(Visual::Effect::SPAWN_POS_RAND_RANGE) - Visual::Effect::SPAWN_POS_RAND_OFFSET) / 100.0f, sinf(angle) * dist));

			VECTOR pVel = VGet((GetRand(Visual::Effect::SPAWN_POS_RAND_RANGE) - Visual::Effect::SPAWN_POS_RAND_OFFSET) / Visual::Effect::SPAWN_VEL_XZ_DIVISOR, (GetRand(Visual::Effect::SPAWN_POS_RAND_RANGE) - Visual::Effect::SPAWN_VEL_Y_OFFSET) / Visual::Effect::SPAWN_VEL_Y_DIVISOR, (GetRand(Visual::Effect::SPAWN_POS_RAND_RANGE) - Visual::Effect::SPAWN_POS_RAND_OFFSET) / Visual::Effect::SPAWN_VEL_XZ_DIVISOR);

			float pLife = Visual::Effect::SPAWN_LIFE_BASE + (GetRand(Visual::Effect::SPAWN_LIFE_RAND_RANGE) / 100.0f);
			float pSize = Visual::Effect::SPAWN_SIZE_BASE + (GetRand(Visual::Effect::SPAWN_SIZE_RAND_RANGE) / 100.0f);

			int pColor = (GetRand(1) == 0) ? GetColor(Global::Palette::WHITE.r, Global::Palette::WHITE.g, Global::Palette::WHITE.b) : GetColor(Global::Palette::VIOLET.r, Global::Palette::VIOLET.g, Global::Palette::VIOLET.b);

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
			it->vel.x *= std::pow(Visual::Effect::SPAWN_VEL_DECAY, dt60);
			it->vel.z *= std::pow(Visual::Effect::SPAWN_VEL_DECAY, dt60);
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
	int debrisCount = Visual::Effect::EXPLOSION_DEBRIS_COUNT_MIN + GetRand(Visual::Effect::EXPLOSION_DEBRIS_COUNT_RAND);
	for (int i = 0; i < debrisCount; ++i) {
		float theta = (GetRand(100) / 100.0f) * DX_PI_F * 2.0f;
		float phi = acosf(1.0f - 2.0f * (GetRand(100) / 100.0f));
		float speed = (GetRand(100) / 100.0f) * Visual::Effect::EXPLOSION_SPEED_RAND_MULT + Visual::Effect::EXPLOSION_SPEED_BASE;

		d.vel.x = sinf(phi) * cosf(theta) * speed;
		d.vel.y = cosf(phi) * speed;
		d.vel.z = sinf(phi) * sinf(theta) * speed;

		d.rotAxis = VNorm(VGet(GetRand(Visual::Effect::ROT_AXIS_RAND_RANGE) - Visual::Effect::ROT_AXIS_RAND_OFFSET, GetRand(Visual::Effect::ROT_AXIS_RAND_RANGE) - Visual::Effect::ROT_AXIS_RAND_OFFSET, GetRand(Visual::Effect::ROT_AXIS_RAND_RANGE) - Visual::Effect::ROT_AXIS_RAND_OFFSET));
		if (VSize(d.rotAxis) < System::Collision::MIN_DIST_SQUARED) d.rotAxis = VGet(0.0f, 1.0f, 0.0f);
		d.angle = 0.0f;
		d.rotSpeed = (GetRand(100) / 100.0f) * Visual::Effect::EXPLOSION_ROT_SPEED_RAND_MULT + Visual::Effect::EXPLOSION_ROT_SPEED_BASE;
		d.size = Visual::Effect::EXPLOSION_SIZE_BASE + (GetRand(Visual::Effect::EXPLOSION_SIZE_RAND_RANGE) / 100.0f);

		debrisList.push_back(d);
	}
}

void ExplosionEffect::Update() {
	float dt = Time::GetIns().GetDelta();
	float dt60 = dt * Global::Math::FPS_BASE;

	lifeTime -= dt;
	if (lifeTime <= 0.0f) {
		alive = false;
		return;
	}

	for (auto& d : debrisList) {
		d.vel.y -= Visual::Effect::EFFECT_GRAVITY * dt60;
		d.pos = VAdd(d.pos, VScale(d.vel, dt60));

		if (d.pos.y - d.size < Global::World::Y_MIN) {
			d.pos.y = Global::World::Y_MIN + d.size;
			d.vel.y *= Visual::Effect::PARTICLE_BOUNCE_COEFFICIENT;
			d.vel.x *= std::pow(Visual::Effect::PARTICLE_FRICTION_GROUND_XZ, dt60);
			d.vel.z *= std::pow(Visual::Effect::PARTICLE_FRICTION_GROUND_XZ, dt60);
		}
		else {
			float friction = std::pow(Visual::Effect::PARTICLE_FRICTION_AIR_XZ, dt60);
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
	float coreScale = 1.0f - std::pow(1.0f - coreProgress, Visual::Effect::EXPLOSION_CORE_POW);
	float currentRadius = maxRadius * coreScale;

	int coreAlpha = static_cast<int>(Visual::Effect::EXPLOSION_CORE_ALPHA_MAX * scale);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, coreAlpha);
	DrawSphere3D(position, currentRadius, System::Window::CIRCLE_DIVNUM, color, color, TRUE);

	int debrisAlpha = static_cast<int>(255 * (scale * scale));
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, debrisAlpha);

	for (const auto& d : debrisList) {
		MATRIX rot = MGetRotAxis(d.rotAxis, d.angle);
		VECTOR topOffset = VTransform(VGet(0.0f, d.size, 0.0f), rot);
		VECTOR bottomOffset = VTransform(VGet(0.0f, -d.size, 0.0f), rot);

		VECTOR top = VAdd(d.pos, topOffset);
		VECTOR bottom = VAdd(d.pos, bottomOffset);

		DrawCone3D(top, bottom, d.size, Visual::Effect::CONE_SEGMENTS_LOW, color, color, TRUE);
	}

	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}


HitScanTrail::HitScanTrail(VECTOR start, VECTOR end, int c, float life)
	: Effect(start, life)
	, startPos(start)
	, endPos(end)
	, color(c)
	, initialRad(System::Collision::HITSCAN_RAY_THICKNESS) {}

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
	DrawCapsule3D(currentStart, endPos, currentRad, Visual::Effect::CAPSULE_SEGMENTS_HITSCAN, color, color, TRUE);

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
	DrawCapsule3D(currentStart, endPos, currentRad * Visual::Effect::TRAIL_INNER_RAD_MULT, Visual::Effect::CAPSULE_SEGMENTS_HITSCAN, color, color, TRUE);

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

	DrawCapsule3D(startPos, endPos, currentRadius, Visual::Effect::CAPSULE_SEGMENTS_TRAIL, color, color, TRUE);

	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	SetWriteZBuffer3D(TRUE);
}