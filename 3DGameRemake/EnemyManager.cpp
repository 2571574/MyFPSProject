#include "EnemyManager.h"
#include "Time.h"
#include "Player.h"
#include "MeleeEnemy.h"
#include "RifleEnemy.h"
#include "SniperEnemy.h"
#include "RollingEnemy.h"
#include <algorithm>
#include "Debug.h"

EnemyManager& EnemyManager::GetIns() {
	static EnemyManager ins;
	return ins;
}
void EnemyManager::Init(int modelhandle,Player * _target){
	stageHandle = modelhandle;
	InitNode(stageHandle,mapNode);
	target = _target;
	maxEnemiesOnMap = 15;
	currentSpawnInterval = 5.0f;
	minSpawnInterval = 0.2f;
	spawnTimer = currentSpawnInterval;
	difficultyTimer = 0.0f;
	prepareTimer = 10.0f;
	maxLimitMelee = 6;
	maxLimitRifle = 4;
	maxLimitSniper = 3;
	maxLimitRolling = 2;

	EnemyManager::GetIns().AddSpawnPoint(VGet(25.0f, 2.0f, 25.0f));
	EnemyManager::GetIns().AddSpawnPoint(VGet(-25.0f, 2.0f, 25.0f));
	EnemyManager::GetIns().AddSpawnPoint(VGet(27.0f, 2.0f, -27.0f));
	EnemyManager::GetIns().AddSpawnPoint(VGet(-27.0f, 2.0f, -27.0f));
	EnemyManager::GetIns().AddSpawnPoint(VGet(12.0f, 10.0f, 12.0f));
	EnemyManager::GetIns().AddSpawnPoint(VGet(-12.0f, 10.0f, -12.0f));
	EnemyManager::GetIns().AddSpawnPoint(VGet(-6.0f, 10.0f, 12.0f));
	EnemyManager::GetIns().AddSpawnPoint(VGet(6.0f, 10.0f, -12.0f));
	EnemyManager::GetIns().AddSpawnPoint(VGet(0.0f, 22.0f, -25.0f));
	EnemyManager::GetIns().AddSpawnPoint(VGet(27.0f, 22.0f, -27.0f));
	EnemyManager::GetIns().AddSpawnPoint(VGet(-27.0f, 22.0f, -27.0f));
	
}

int EnemyManager::CountEnemyType(ENEMYTYPE type) {
	int count = 0;
	for (const auto& e : enemies) {
		if (e->GetType() == type)count++;
	}
	return count;
}


void EnemyManager::Spawn(std::unique_ptr<Enemy>enemy) {
	enemy->SetStageHandle(stageHandle);
	if (enemy)enemies.push_back(std::move(enemy));
}


int EnemyManager::Update() {
	float dt = Time::GetIns().GetDelta();
	int earnScore = 0;
	//配列の敵を1体ずつ更新させる
	for (int i = enemies.size() - 1; i >= 0; i--) {
		enemies[i]->Update();
		Debug::Watch("BotHP", enemies[i]->GetHP());

		if (enemies[i]->CheckFall()) {
			enemies[i]->SetAlive(false);
		}

		//生存タグが消えたら消去する
		if (!enemies[i]->IsAlive()) {
			earnScore += enemies[i]->GetStatus().score;
			enemies[i] = std::move(enemies.back());
			enemies.pop_back();
		}
	}

	if (target == nullptr || spawnPoints.empty())return earnScore;

	if (prepareTimer > 0.0f) {
		prepareTimer -= dt;
		return earnScore;
	}
	difficultyTimer += dt;
	if (difficultyTimer >= 10.0f) {
		difficultyTimer = 0.0f;
		currentSpawnInterval -= 0.2f;
		if (currentSpawnInterval < minSpawnInterval) {
			currentSpawnInterval = minSpawnInterval;
		}
	}

	spawnTimer -= dt;
	if (spawnTimer <= 0.0f) {
		spawnTimer = currentSpawnInterval;

		int spawnCount = (currentSpawnInterval <= 0.5f) ? 1 : 3;

		for (int i = 0; i < spawnCount; i++) {
			if (enemies.size() >= maxEnemiesOnMap)break;

			int index = GetRand((int)spawnPoints.size() - 1);
			VECTOR sPos = spawnPoints[index];

			VECTOR checkStart = VGet(sPos.x, sPos.y + 5.0f, sPos.z);
			VECTOR checkEnd = VGet(sPos.x, sPos.y - 10.0f, sPos.z);
			MV1_COLL_RESULT_POLY ground = MV1CollCheck_Line(stageHandle, -1, checkStart, checkEnd);

			if(ground.HitFlag == 1 && ground.Normal.y > 0.3f){
				sPos.y = ground.HitPosition.y;
			}
			else {
				continue;
			}
			if (VSize(VSub(target->GetPos(), sPos)) < 10.0f)continue;

			bool isSafe = true;

			for (const auto& e : enemies) {
				if (VSize(VSub(e->GetPos(), sPos)) < 2.0f) {
					isSafe = false;
					break;
				}
			}
			if (!isSafe)continue;

			std::vector<ENEMYTYPE>availableTypes;
			if (CountEnemyType(ENEMYTYPE::MELEE) < maxLimitMelee)
				availableTypes.push_back(ENEMYTYPE::MELEE);
			if (CountEnemyType(ENEMYTYPE::RIFLE) < maxLimitMelee)
				availableTypes.push_back(ENEMYTYPE::RIFLE);
			if (CountEnemyType(ENEMYTYPE::SNIPER) < maxLimitMelee)
				availableTypes.push_back(ENEMYTYPE::SNIPER);
			if (CountEnemyType(ENEMYTYPE::ROLLING) < maxLimitMelee)
				availableTypes.push_back(ENEMYTYPE::ROLLING);

			if (availableTypes.empty())break;

			ENEMYTYPE selectedType = availableTypes[GetRand((int)availableTypes.size() - 1)];

			std::unique_ptr<Enemy> newEnemy = nullptr;

			switch (selectedType) {
			case ENEMYTYPE::MELEE: newEnemy = std::make_unique<MeleeEnemy>(sPos, target); break;
			case ENEMYTYPE::RIFLE: newEnemy = std::make_unique<RifleEnemy>(sPos, target); break;
			case ENEMYTYPE::SNIPER: newEnemy = std::make_unique<SniperEnemy>(sPos, target); break;
			case ENEMYTYPE::ROLLING: newEnemy = std::make_unique<RollingEnemy>(sPos, target); break;
			default: break;
			}

			if (newEnemy) {
				Spawn(std::move(newEnemy));
				Debug::Log("Spawned");
			}
		}
	}

	return earnScore;
}


void EnemyManager::Draw() {
	for (const auto& e : enemies) {
		e->Draw();		//1体ずつ描画　繰り返し
	}
}

void EnemyManager::Clear() {
	enemies.clear();
}

Enemy* EnemyManager::CheckHitScan(VECTOR start, VECTOR end, TEAMID shooterTeam) {
	Enemy* hitEnemy = nullptr;		//当たった敵のポインタ
	float minDistance = FLT_MAX;	//最短の敵との距離

	//1体ずつ繰り返し
	for (const auto& e : enemies) {
		if (e->GetID() == shooterTeam)continue;	//射手とIDが一緒なら飛ばす

		//敵の判定のカプセルを作成
		VECTOR ePos = e->GetPos();
		VECTOR eTop = VAdd(ePos, VGet(0, e->GetStatus().height, 0));
		float r = e->GetRadius();

		//弾をカプセルに見立ててチェック
		if (HitCheck_Capsule_Capsule(start, end, 0.05f, ePos, eTop, r)) {
			//当たった場合敵と発射地点との距離をチェック
			float dist = VSize(VSub(ePos, start));

			//最短距離なら距離とポインタを上書きする
			if (dist < minDistance) {
				minDistance = dist;
				hitEnemy = e.get();
			}
		}
	}
	return hitEnemy;
}

Enemy* EnemyManager::CheckProjectile(VECTOR pos, VECTOR nextpos, float radius, TEAMID shooterTeam) {
	for (const auto& e : enemies) {
		if (e->GetID() == shooterTeam)continue;	//射手とIDが一緒なら飛ばす

		//敵の判定のカプセルを作成
		VECTOR ePos = e->GetPos();
		VECTOR eTop = VAdd(ePos, VGet(0, e->GetStatus().height, 0));
		float eRad = e->GetStatus().width / 2.0f;

		//現在の弾と次のフレームの弾をつないだカプセルとして判定をチェック
		if (HitCheck_Capsule_Capsule(pos, nextpos, radius, ePos, eTop, eRad)) {
			return e.get();
		}
	}
	return nullptr;
}


std::vector<VECTOR> EnemyManager::CalculatePath(VECTOR startPos, VECTOR goalPos) {
	return FindPath(startPos, goalPos, mapNode);
}

