#include "CollisionManager.h"
#include "Character.h"
#include <algorithm>

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

void CollisionManager::Update(Player* player, EnemyManager* enemymanager) {
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

			if (distSq > 0.01f && distSq < sumRad * sumRad) {
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
		}
	}

	for (auto* chara : characters) {
		if (chara->GetID() == shooter || !chara->IsAlive())continue;

		VECTOR cPos = chara->GetPos();

		float headRad =0.25f;
		VECTOR headPos = VAdd(cPos, VGet(0.0f, chara->GetCurrentEyeHeight(), 0.0f));
		if (HitCheck_Capsule_Capsule(start, end, 0.05f, headPos, headPos, headRad)) {
			float dist = VSize(VSub(headPos, start));
			if (dist < minDistance) {
				minDistance = dist;
				result.character = chara;
				result.isHeadShot = true;
				result.isWallHit = false;
				result.hitPos = headPos;
			}
			continue;
		}

		VECTOR bodyTop = VAdd(cPos, VGet(0.0f, chara->GetStatus().height - headRad, 0.0f));
		float bodyRad = chara->GetStatus().width / 2.0f;


		if (HitCheck_Capsule_Capsule(start, end, 0.05f, cPos, bodyTop, bodyRad)) {
			float dist = VSize(VSub(cPos, start));
			if (dist < minDistance) {
				minDistance = dist;
				result.character = chara;
				result.isHeadShot = false;
				result.isWallHit = false;
				result.hitPos = cPos;
			}
		}
	}

	return result;
}


HitInfo CollisionManager::CheckProjectile(VECTOR pos, VECTOR nextPos, float radius, TEAMID shooter) {
	HitInfo result;
	result.hitPos = nextPos;
	float minDistance = VSize(VSub(nextPos, pos));

	if (stageHandle != -1) {
		MV1_COLL_RESULT_POLY wallHit = MV1CollCheck_Line(stageHandle, -1, pos, nextPos);

		if (wallHit.HitFlag == 1) {
			minDistance = VSize(VSub(wallHit.HitPosition, pos));
			result.isWallHit = true;
			result.hitPos = wallHit.HitPosition;
		}
	}


	for (auto* chara : characters) {
		if (chara->GetID() == shooter || !chara->IsAlive())continue;

		VECTOR cPos = chara->GetPos();
		float headRad =0.25f;
		VECTOR headPos = VAdd(cPos, VGet(0.0f, chara->GetCurrentEyeHeight(), 0.0f));
		if (HitCheck_Capsule_Capsule(pos, nextPos, radius, headPos, headPos, headRad)) {
			result.character = chara;
			result.isHeadShot = true;
			result.isWallHit = false;
			result.hitPos = headPos;
			return result;
		}
		float bodyRad = chara->GetStatus().width / 2.0f;
		VECTOR bodyBottom = VAdd(cPos, VGet(0.0f, bodyRad, 0.0f));
		VECTOR bodyTop = VAdd(cPos, VGet(0.0f, chara->GetStatus().height - bodyRad, 0.0f));
		if (HitCheck_Capsule_Capsule(pos, nextPos, radius, bodyBottom, bodyTop, bodyRad)) {
			result.character = chara;
			result.isHeadShot = false;
			result.isWallHit = false;
			result.hitPos = cPos;
			return result;
		}
	}
	return result;
}

bool CollisionManager::ProcessExplotion(VECTOR hitPos, float radius, int damage,float knockbackPower,bool distanceFallOff, TEAMID shooter, WeaponID id) {
	if (radius <= 0.0f)return false;
	bool hit = false;

	for (auto* chara : characters) {
		if (!chara->IsAlive())continue;

		if (chara->GetID() == shooter)continue;

		float dist = VSize(VSub(chara->GetPos(), hitPos));
		if (dist <= radius) {
			hit = true;
			float damageRate = distanceFallOff ? (1.0f - (dist / radius)) : 1.0f;
			int actualDamage = (int)(damage * damageRate);
			if (actualDamage < 1 && damage > 0)actualDamage = 1;


			chara->OnHit((int)(damage * damageRate),id);

			VECTOR toChara = VSub(chara->GetPos(), hitPos);

			if (VSize(toChara) < 0.01f) {
				toChara = VGet(0.0f, 1.0f, 0.0f);
			}
			else {
				toChara = VNorm(toChara);
				toChara.y += 0.5f;
				toChara = VNorm(toChara);
			}

			float currentKnockback = knockbackPower * damageRate;
			
			chara->Applyknockback(VScale(toChara, currentKnockback));
		}
	}
	return hit;
}