#include "EnemyManager.h"
#include <algorithm>

EnemyManager& EnemyManager::GetIns() {
	static EnemyManager ins;
	return ins;
}
void EnemyManager::Init(int modelhandle){
	InitNode(modelhandle,mapNode,pair);
}
void EnemyManager::Spawn(std::unique_ptr<Enemy>enemy,int stageHandle) {
	enemy->SetStageHandle(stageHandle);
	if (enemy)enemies.push_back(std::move(enemy));
}


void EnemyManager::Update() {
	//配列の敵を1体ずつ更新させる
	for (int i = enemies.size() - 1; i >= 0; i--) {
		enemies[i]->Update();
		Debug::Watch("BotHP", enemies[i]->GetHP());

		//生存タグが消えたら消去する
		if (!enemies[i]->IsAlive()) {
			enemies[i] = std::move(enemies.back());
			enemies.pop_back();
		}
	}
}


void EnemyManager::Draw() {
	for (const auto& e : enemies) {
		e->Draw();		//1体ずつ描画　繰り返し
	}
	for(int i = 0; i < mapNode.size(); i++) {
		//ノードの位置に球を描画
		DrawSphere3D(mapNode[i].position, 0.3f, 8, GetColor(255, 0, 0), GetColor(255, 0, 0), TRUE);
	}
	for (int i = 0; i < pair.size(); i++) {
		SetUseZBuffer3D(false);
		DrawLine3D(pair[i].pos1, pair[i].pos2, GetColor(255, 0, 255));
		SetUseZBuffer3D(true);
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

