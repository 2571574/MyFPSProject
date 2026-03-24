#include "CollisionManager.h"
#include "Character.h"

#include <algorithm>
#include <cfloat>

constexpr float MIN_HEAD_RAD = 0.15f;			//頭の判定の最小
constexpr float HITSCAN_RAY_THICKNESS = 0.05f;	//ヒットスキャンの判定の太さ
constexpr float MIN_DIST_SQUARED = 0.01f;		//押し出し判定などの最小距離の2乗
constexpr float EXPLODE_UPWARD_BIAS = 0.5f;		//ノックバックを上に向かせる補正値

CollisionManager& CollisionManager::GetIns() {
	static CollisionManager ins;
	return ins;
}

void CollisionManager::Register(Character* chara) {
	if(chara){
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

			if (distSq > MIN_DIST_SQUARED && distSq < sumRad * sumRad) {
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
					float half = overlap * 0.5f;
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
		if (wallHit.HitFlag == 1) {
			minDistance = VSize(VSub(wallHit.HitPosition, start));
			result.isWallHit = true;
			result.hitPos = wallHit.HitPosition;
			result.hitNormal = wallHit.Normal;
		}
	}

	for (auto* chara : characters) {
		if (chara->GetID() == shooter || !chara->IsAlive())continue;

		VECTOR cPos = chara->GetPos();
		float hitY = start.y;
		if (hitY < cPos.y)hitY = cPos.y;
		if (hitY > cPos.y + chara->GetStatus().height)hitY = cPos.y + chara->GetStatus().height;
		VECTOR approxBodyHitPos = VGet(cPos.x, hitY, cPos.z);

		float bodyRad = chara->GetStatus().width / 2.0f;
		float headRad =bodyRad / 2.0f;
		if (headRad < MIN_HEAD_RAD)headRad = MIN_HEAD_RAD;
		VECTOR headPos = VAdd(cPos, VGet(0.0f, chara->GetCurrentEyeHeight(), 0.0f));

		if (HitCheck_Capsule_Capsule(start, end, HITSCAN_RAY_THICKNESS, headPos, headPos, headRad)) {
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
		if (neckHeight < bodyRad * 2.0f)neckHeight = bodyRad * 2.0f;
		VECTOR bodyTop = VAdd(cPos, VGet(0.0f, neckHeight - bodyRad, 0.0f));

		if (HitCheck_Capsule_Capsule(start, end, HITSCAN_RAY_THICKNESS, bodyBottom, bodyTop, bodyRad)) {
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

		if (wallHit.HitFlag == 1) {
			minDistance = VSize(VSub(wallHit.HitPosition, pos));
			result.isWallHit = true;
			result.hitPos = wallHit.HitPosition;
			result.hitNormal = wallHit.Normal;
		}
	}


	for (auto* chara : characters) {
		if (chara->GetID() == shooter || !chara->IsAlive())continue;

		VECTOR cPos = chara->GetPos();
		float hitY = pos.y;
		if (hitY < cPos.y)hitY = cPos.y;
		if (hitY > cPos.y + chara->GetStatus().height)hitY = cPos.y + chara->GetStatus().height;
		VECTOR approxBodyHitPos = VGet(cPos.x, hitY, cPos.z);

		float bodyRad = chara->GetStatus().width / 2.0f;
		float headRad = bodyRad / 2.0f;
		if (headRad < MIN_HEAD_RAD)headRad = MIN_HEAD_RAD;
		VECTOR headPos = VAdd(cPos, VGet(0.0f, chara->GetCurrentEyeHeight(), 0.0f));

		VECTOR moveVec = VSub(nextPos, pos);
		VECTOR extendedNextPos = nextPos;
		if (VSquareSize(moveVec) > MIN_DIST_SQUARED) {
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
		if (neckHeight < bodyRad * 2.0f)neckHeight = bodyRad * 2.0f;

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

bool CollisionManager::ProcessExplosion(VECTOR hitPos, float radius, int damage,float knockbackPower,bool distanceFallOff, TEAMID shooter, WeaponID id,bool friendlyFire) {
	if (radius <= 0.0f)return false;
	bool hit = false;

	for (auto* chara : characters) {
		if (!chara->IsAlive())continue;

		if (!friendlyFire) {
			if (chara->GetID() == shooter)continue;
		}
		float dist = VSize(VSub(chara->GetPos(), hitPos));
		if (dist <= radius) {
			hit = true;
			float damageRate = distanceFallOff ? (1.0f - (dist / radius)) : 1.0f;
			int actualDamage = (int)(damage * damageRate);
			if (actualDamage < 1 && damage > 0)actualDamage = 1;


			chara->OnHit((int)(damage * damageRate),id);

			VECTOR toChara = VSub(chara->GetPos(), hitPos);

			if (VSize(toChara) < MIN_DIST_SQUARED) {
				toChara = VGet(0.0f, 1.0f, 0.0f);
			}
			else {
				toChara = VNorm(toChara);
				toChara.y += EXPLODE_UPWARD_BIAS;
				toChara = VNorm(toChara);
			}

			float currentKnockback = knockbackPower * damageRate;
			
			chara->Applyknockback(VScale(toChara, currentKnockback));
		}
	}
	return hit;
}