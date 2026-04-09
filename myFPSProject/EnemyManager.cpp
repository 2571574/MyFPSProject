#include "EnemyManager.h"
#include "Time.h"
#include "Player.h"
#include "MeleeEnemy.h"
#include "RifleEnemy.h"
#include "SniperEnemy.h"
#include "RollingEnemy.h"
#include "Debug.h"
#include "EffectManager.h"
#include "SoundManager.h"
#include "Status.h"
#include "Param/Global.h"
#include "Param/Chara.h"
#include "Param/System.h"

#include <algorithm>


EnemyManager& EnemyManager::GetIns() {
	static EnemyManager ins;
	return ins;
}

void EnemyManager::Init(int modelhandle, Player* _target) {
	stageHandle = modelhandle;
	InitNode(stageHandle, mapNode);
	target = _target;
	currentSpawnInterval = GameConfig.initSpawnInterval;
	spawnTimer = 0.0f;
	difficultyTimer = 0.0f;
	prepareTimer = GameConfig.prepareTime;
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

	bool isTutorial = (target && target->GetCurrentMode() == PlayMode::MODE_TUTORIAL);

	if (!isTutorial) {
		VECTOR ePos = enemy->GetPos();
		float eHeight = enemy->GetCurrentHeight();
		float eRadius = enemy->GetRadius();

		EffectManager::GetIns().CreateSpawnEffect(ePos, eHeight, eRadius, Chara::EnemyCommon::SPAWN_PROCESS_TIME);
	}
	else {
		enemy->SkipSpawnPhase();
	}
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
		if (!enemies[i]->GetAlive()) {
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
	if (target->GetCurrentMode() == PlayMode::MODE_TUTORIAL) return earnScore;

	//準備時間中スポーンしない
	if (prepareTimer > 0.0f) {
		prepareTimer -= dt;
		return earnScore;
	}

	//難易度上昇
	difficultyTimer += dt;
	if (difficultyTimer >= GameConfig.difficultyUpInterval) {
		difficultyTimer = 0.0f;
		currentSpawnInterval -= GameConfig.difficultyUpAmount;
		if (currentSpawnInterval < GameConfig.minSpawnInterval) {
			currentSpawnInterval = GameConfig.minSpawnInterval;
		}
	}

	//スポーン処理
	spawnTimer -= dt;
	if (spawnTimer <= 0.0f) {
		spawnTimer = currentSpawnInterval;

		//沸き間隔が長いときは3体ずつスポーン
		int spawnCount = (currentSpawnInterval <= Chara::EnemyManager::BURST_SPAWN_THRESHOLD) ? 1 : Chara::EnemyManager::NORMAL_SPAWNCOUNT;

		for (int i = 0; i < spawnCount; i++) {
			if (enemies.size() >= GameConfig.maxEnemyOnMap)break;

			int index = GetRand((int)spawnPoints.size() - 1);
			VECTOR sPos = spawnPoints[index];

			//地面に密着させてスポーン
			VECTOR checkStart = VGet(sPos.x, sPos.y + Chara::EnemyManager::SPAWN_RAY_START, sPos.z);
			VECTOR checkEnd = VGet(sPos.x, sPos.y + Chara::EnemyManager::SPAWN_RAY_END, sPos.z);
			MV1_COLL_RESULT_POLY ground = MV1CollCheck_Line(stageHandle, -1, checkStart, checkEnd);

			if (ground.HitFlag == TRUE && ground.Normal.y > Chara::Base::GROUND_NORMAL_MIN) {
				sPos.y = ground.HitPosition.y;
			}
			else {
				continue;
			}
			if (VSize(VSub(target->GetPos(), sPos)) < Chara::EnemyManager::SPAWNDIST_PLAYER)continue;

			bool isSafe = true;

			for (const auto& e : enemies) {
				if (VSize(VSub(e->GetPos(), sPos)) < Chara::EnemyManager::SPAWNDIST_ENEMY) {
					isSafe = false;
					break;
				}
			}
			if (!isSafe)continue;

			std::vector<ENEMYTYPE>availableTypes;
			if (CountEnemyType(ENEMYTYPE::MELEE) < GameConfig.maxLimitMelee)
				availableTypes.push_back(ENEMYTYPE::MELEE);
			if (CountEnemyType(ENEMYTYPE::RIFLE) < GameConfig.maxLimitRifle)
				availableTypes.push_back(ENEMYTYPE::RIFLE);
			if (CountEnemyType(ENEMYTYPE::SNIPER) < GameConfig.maxLimitSniper)
				availableTypes.push_back(ENEMYTYPE::SNIPER);
			if (CountEnemyType(ENEMYTYPE::ROLLING) < GameConfig.maxLimitRolling)
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
				SoundManager::GetIns().Play3DSE("Resource/Sound/spawn.ogg", newEnemy->GetPos(), Chara::EnemyManager::SPAWN_SOUND_RADIUS);
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

