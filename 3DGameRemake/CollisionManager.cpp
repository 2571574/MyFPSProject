#include "CollisionManager.h"
#include "Character.h"
#include "Param/System.h"

#include <algorithm>
#include <cfloat>

CollisionManager& CollisionManager::GetIns() {
	static CollisionManager ins;
	return ins;
}

void CollisionManager::Register(Character* chara) {
	if (chara) {
		characters.push_back(chara);
	}
}

void CollisionManager::Unregister(Character* chara) {
	if (chara) {
		auto t = std::find(characters.begin(), characters.end(), chara);
		if (t != characters.end()) {
			characters.erase(t);
		}
	}
}

void CollisionManager::Update() {
	for (size_t i = 0; i < characters.size(); ++i) {
		if (!characters[i]->IsAlive())continue;

		for (size_t j = i + 1; j < characters.size(); ++j) {
			if (!characters[j]->IsAlive())continue;

			Character* charaA = characters[i];
			Character* charaB = characters[j];

			VECTOR posA = charaA->GetPos();
			VECTOR posB = charaB->GetPos();

			float heightA = charaA->GetStatus().height;
			float heightB = charaB->GetStatus().height;
			if (posA.y > posB.y + heightB || posB.y > posA.y + heightA) {
				continue;
			}

			VECTOR vecAB = VSub(posB, posA);
			vecAB.y = 0.0f;

			float distSq = VSquareSize(vecAB);
			float radA = charaA->GetStatus().width / 2.0f;
			float radB = charaB->GetStatus().width / 2.0f;

			float sumRad = radA + radB;

			if (distSq > System::Collision::MIN_DIST_SQUARED && distSq < sumRad * sumRad) {
				float dist = sqrtf(distSq);
				float overlap = sumRad - dist;
				VECTOR pushDir = VScale(vecAB, 1.0f / dist);

				int massA = charaA->GetStatus().mass;
				int massB = charaB->GetStatus().mass;

				if (massA > massB) {
					charaB->SetPos(VAdd(posB, VScale(pushDir, overlap)));
				}
				else if (massB > massA) {
					charaA->SetPos(VSub(posA, VScale(pushDir, overlap)));
				}
				else {
					float half = overlap * System::Collision::PUSH_SPLIT_RATIO_EQUAL_MASS;
					charaA->SetPos(VSub(posA, VScale(pushDir, half)));
					charaB->SetPos(VAdd(posB, VScale(pushDir, half)));
				}
			}
		}
	}
}

HitInfo CollisionManager::CheckHitScan(VECTOR start, VECTOR end, TEAMID shooter) {
	HitInfo result;
	float minDistance = FLT_MAX;
	if (stageHandle != -1) {
		MV1_COLL_RESULT_POLY wallHit = MV1CollCheck_Line(stageHandle, -1, start, end);
		if (wallHit.HitFlag == TRUE) {
			minDistance = VSize(VSub(wallHit.HitPosition, start));
			result.isWallHit = true;
			result.hitPos = wallHit.HitPosition;
			result.hitNormal = wallHit.Normal;
		}
	}

	for (auto* chara : characters) {
		if (chara->GetID() == shooter || !chara->IsAlive() || chara->GetHP() <= 0) continue;

		VECTOR cPos = chara->GetPos();
		float hitY = start.y;
		if (hitY < cPos.y)hitY = cPos.y;
		if (hitY > cPos.y + chara->GetStatus().height)hitY = cPos.y + chara->GetStatus().height;
		VECTOR approxBodyHitPos = VGet(cPos.x, hitY, cPos.z);

		float bodyRad = chara->GetStatus().width / 2.0f;
		float headRad = bodyRad / 2.0f;
		if (headRad < System::Collision::MIN_HEAD_RAD)headRad = System::Collision::MIN_HEAD_RAD;
		VECTOR headPos = VAdd(cPos, VGet(0.0f, chara->GetCurrentEyeHeight(), 0.0f));

		if (HitCheck_Capsule_Capsule(start, end, System::Collision::HITSCAN_RAY_THICKNESS, headPos, headPos, headRad)) {
			float dist = VSize(VSub(headPos, start));
			if (dist < minDistance) {
				minDistance = dist;
				result.character = chara;
				result.isHeadShot = true;
				result.isWallHit = false;
				result.hitPos = headPos;
				result.hitNormal = VNorm(VSub(start, end));
			}
			continue;
		}

		VECTOR bodyBottom = VAdd(cPos, VGet(0.0f, bodyRad, 0.0f));
		float neckHeight = chara->GetCurrentEyeHeight() - headRad;
		if (neckHeight < bodyRad * System::Collision::MIN_NECK_HEIGHT_BODYRAD_MULT)neckHeight = bodyRad * System::Collision::MIN_NECK_HEIGHT_BODYRAD_MULT;
		VECTOR bodyTop = VAdd(cPos, VGet(0.0f, neckHeight - bodyRad, 0.0f));

		if (HitCheck_Capsule_Capsule(start, end, System::Collision::HITSCAN_RAY_THICKNESS, bodyBottom, bodyTop, bodyRad)) {
			float dist = VSize(VSub(approxBodyHitPos, start));
			if (dist < minDistance) {
				minDistance = dist;
				result.character = chara;
				result.isHeadShot = false;
				result.isWallHit = false;
				result.hitPos = approxBodyHitPos;
				result.hitNormal = VNorm(VSub(start, end));
			}
		}
	}

	return result;
}

HitInfo CollisionManager::CheckProjectile(VECTOR pos, VECTOR nextPos, float radius, TEAMID shooter) {
	HitInfo result;
	result.hitPos = nextPos;
	float minDistance = FLT_MAX;

	if (stageHandle != -1) {
		MV1_COLL_RESULT_POLY wallHit = MV1CollCheck_Line(stageHandle, -1, pos, nextPos);

		if (wallHit.HitFlag == TRUE) {
			minDistance = VSize(VSub(wallHit.HitPosition, pos));
			result.isWallHit = true;
			result.hitPos = wallHit.HitPosition;
			result.hitNormal = wallHit.Normal;
		}
	}

	for (auto* chara : characters) {
		if (chara->GetID() == shooter || !chara->IsAlive() || chara->GetHP() <= 0) continue;

		VECTOR cPos = chara->GetPos();
		float hitY = pos.y;
		if (hitY < cPos.y)hitY = cPos.y;
		if (hitY > cPos.y + chara->GetStatus().height)hitY = cPos.y + chara->GetStatus().height;
		VECTOR approxBodyHitPos = VGet(cPos.x, hitY, cPos.z);

		float bodyRad = chara->GetStatus().width / 2.0f;
		float headRad = bodyRad / 2.0f;
		if (headRad < System::Collision::MIN_HEAD_RAD)headRad = System::Collision::MIN_HEAD_RAD;
		VECTOR headPos = VAdd(cPos, VGet(0.0f, chara->GetCurrentEyeHeight(), 0.0f));

		VECTOR moveVec = VSub(nextPos, pos);
		VECTOR extendedNextPos = nextPos;
		if (VSquareSize(moveVec) > System::Collision::MIN_DIST_SQUARED) {
			VECTOR moveDir = VNorm(moveVec);

			extendedNextPos = VAdd(nextPos, VScale(moveDir, chara->GetStatus().width));
		}
		if (HitCheck_Capsule_Capsule(pos, extendedNextPos, radius, headPos, headPos, headRad)) {
			float dist = VSize(VSub(headPos, pos));
			if (dist < minDistance) {
				minDistance = dist;
				result.character = chara;
				result.isHeadShot = true;
				result.isWallHit = false;
				result.hitPos = headPos;
				result.hitNormal = VNorm(VSub(pos, nextPos));
			}
			continue;
		}
		VECTOR bodyBottom = VAdd(cPos, VGet(0.0f, bodyRad, 0.0f));
		float neckHeight = chara->GetCurrentEyeHeight() - headRad;
		if (neckHeight < bodyRad * System::Collision::MIN_NECK_HEIGHT_BODYRAD_MULT)neckHeight = bodyRad * System::Collision::MIN_NECK_HEIGHT_BODYRAD_MULT;

		VECTOR bodyTop = VAdd(cPos, VGet(0.0f, neckHeight - bodyRad, 0.0f));
		if (HitCheck_Capsule_Capsule(pos, nextPos, radius, bodyBottom, bodyTop, bodyRad)) {
			float dist = VSize(VSub(approxBodyHitPos, pos));
			if (dist < minDistance) {
				minDistance = dist;
				result.character = chara;
				result.isHeadShot = false;
				result.isWallHit = false;
				result.hitPos = approxBodyHitPos;
				result.hitNormal = VNorm(VSub(pos, nextPos));
			}
		}
	}
	return result;
}

bool CollisionManager::ProcessExplosion(VECTOR hitPos, float radius, int damage, float knockbackPower, bool distanceFallOff, TEAMID shooter, WeaponID id, bool friendlyFire) {
	if (radius <= 0.0f)return false;
	bool hit = false;

	for (auto* chara : characters) {
		if (!chara->IsAlive() || chara->GetHP() <= 0) continue;

		if (!friendlyFire) {
			if (chara->GetID() == shooter)continue;
		}
		float dist = VSize(VSub(chara->GetPos(), hitPos));
		if (dist <= radius) {
			hit = true;
			float damageRate = distanceFallOff ? (1.0f - (dist / radius)) : 1.0f;
			int actualDamage = (int)(damage * damageRate);
			if (actualDamage < System::Collision::MIN_DAMAGE_ON_FALLOFF && damage > 0)actualDamage = System::Collision::MIN_DAMAGE_ON_FALLOFF;

			chara->OnHit(actualDamage, id);

			VECTOR toChara = VSub(chara->GetPos(), hitPos);

			if (VSize(toChara) < System::Collision::MIN_DIST_SQUARED) {
				toChara = VGet(0.0f, System::Collision::DEFAULT_FALLBACK_DIR_Y, 0.0f);
			}
			else {
				toChara = VNorm(toChara);
				toChara.y += System::Collision::EXPLODE_UPWARD_BIAS;
				toChara = VNorm(toChara);
			}

			float currentKnockback = knockbackPower * damageRate;

			chara->Applyknockback(VScale(toChara, currentKnockback));
		}
	}
	return hit;
}