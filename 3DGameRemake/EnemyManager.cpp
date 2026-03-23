#include "EnemyManager.h"
#include "Time.h"
#include "Player.h"
#include "MeleeEnemy.h"
#include "RifleEnemy.h"
#include "SniperEnemy.h"
#include "RollingEnemy.h"
#include "Debug.h"
#include "EffectManager.h"

#include <algorithm>

namespace {
	constexpr int MAXENEMY_ONMAP = 15;
	constexpr int MAX_LIMIT_MELEE = 6;
	constexpr int MAX_LIMIT_RIFLE = 4;
	constexpr int MAX_LIMIT_SNIPER = 3;
	constexpr int MAX_LIMIT_ROLLING = 2;

	constexpr float INIT_SPAWN_INTERVAL = 8.0f;
	constexpr float PREPARE_TIME = 10.0f;
	constexpr float DIFFICULTY_UP_INTERVAL = 20.0f;
	constexpr float DIFFICULTY_UP_AMOUNT = 0.5f;
	constexpr float MIN_SPAWN_INTERVAL = 0.2f;
	constexpr float BURST_SPAWN_THRESHOLD = 0.5f;
	constexpr int NORMAL_SPAWNCOUNT = 3;

	constexpr float SPAWNDIST_PLAYER = 10.0f;
	constexpr float SPAWNDIST_ENEMY = 2.0f;

	constexpr float SPAWN_RAY_START_OFFSET = 5.0f;
	constexpr float SPAWN_RAY_END_OFFSET = -10.0f;
	constexpr float SPAWN_GROUND_NORMAL_MIN = 0.3f;
}

EnemyManager& EnemyManager::GetIns() {
	static EnemyManager ins;
	return ins;
}

void EnemyManager::Init(int modelhandle,Player * _target){
	stageHandle = modelhandle;
	InitNode(stageHandle,mapNode);
	target = _target;
	currentSpawnInterval = INIT_SPAWN_INTERVAL;
	spawnTimer = 0.0f;
	difficultyTimer = 0.0f;
	prepareTimer = PREPARE_TIME;
	killCounts.clear();
	spawnPoints.clear();

	totalScore = 0;
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
			totalScore += enemies[i]->GetStatus().score;
			killCounts[enemies[i]->GetType()]++;

			VECTOR deathPos = VAdd(enemies[i]->GetPos(), VGet(0.0f, enemies[i]->GetCurrentHeight() * 0.5f, 0.0f));
			float floorY = enemies[i]->GetPos().y;
			EffectManager::GetIns().CreateDeathParticle(deathPos, floorY, enemies[i]->GetType());

			enemies[i] = std::move(enemies.back());
			enemies.pop_back();
		}
	}

	if (target == nullptr || spawnPoints.empty())return earnScore;

	//準備時間中スポーンしない
	if (prepareTimer > 0.0f) {
		prepareTimer -= dt;
		return earnScore;
	}

	//難易度上昇
	difficultyTimer += dt;
	if (difficultyTimer >= DIFFICULTY_UP_INTERVAL) {
		difficultyTimer = 0.0f;
		currentSpawnInterval -= DIFFICULTY_UP_AMOUNT;
		if (currentSpawnInterval < MIN_SPAWN_INTERVAL) {
			currentSpawnInterval = MIN_SPAWN_INTERVAL;
		}
	}

	//スポーン処理
	spawnTimer -= dt;
	if (spawnTimer <= 0.0f) {
		spawnTimer = currentSpawnInterval;

		//沸き間隔が長いときは3体ずつスポーン
		int spawnCount = (currentSpawnInterval <= BURST_SPAWN_THRESHOLD) ? 1 : NORMAL_SPAWNCOUNT;

		for (int i = 0; i < spawnCount; i++) {
			if (enemies.size() >= MAXENEMY_ONMAP)break;

			int index = GetRand((int)spawnPoints.size() - 1);
			VECTOR sPos = spawnPoints[index];

			//地面に密着させてスポーン
			VECTOR checkStart = VGet(sPos.x, sPos.y + SPAWN_RAY_START_OFFSET, sPos.z);
			VECTOR checkEnd = VGet(sPos.x, sPos.y + SPAWN_RAY_END_OFFSET, sPos.z);
			MV1_COLL_RESULT_POLY ground = MV1CollCheck_Line(stageHandle, -1, checkStart, checkEnd);

			if(ground.HitFlag == 1 && ground.Normal.y > SPAWN_GROUND_NORMAL_MIN){
				sPos.y = ground.HitPosition.y;
			}
			else {
				continue;
			}
			if (VSize(VSub(target->GetPos(), sPos)) < SPAWNDIST_PLAYER)continue;

			bool isSafe = true;

			for (const auto& e : enemies) {
				if (VSize(VSub(e->GetPos(), sPos)) < SPAWNDIST_ENEMY) {
					isSafe = false;
					break;
				}
			}
			if (!isSafe)continue;

			std::vector<ENEMYTYPE>availableTypes;
			if (CountEnemyType(ENEMYTYPE::MELEE) < MAX_LIMIT_MELEE)
				availableTypes.push_back(ENEMYTYPE::MELEE);
			if (CountEnemyType(ENEMYTYPE::RIFLE) < MAX_LIMIT_RIFLE)
				availableTypes.push_back(ENEMYTYPE::RIFLE);
			if (CountEnemyType(ENEMYTYPE::SNIPER) < MAX_LIMIT_SNIPER)
				availableTypes.push_back(ENEMYTYPE::SNIPER);
			if (CountEnemyType(ENEMYTYPE::ROLLING) < MAX_LIMIT_ROLLING)
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

std::vector<VECTOR> EnemyManager::CalculatePath(VECTOR startPos, VECTOR goalPos) {
	return FindPath(startPos, goalPos, mapNode);
}

