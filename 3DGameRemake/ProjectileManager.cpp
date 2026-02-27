#include "ProjectileManager.h"
#include"Debug.h"
#include <algorithm>

/*インスタンスを取得*/
ProjectileManager& ProjectileManager::GetIns()
{
	static ProjectileManager ins;
	return ins;
}

/*弾を生成する関数*/
void ProjectileManager::Spawn(std::unique_ptr<BaseProjectile> b)
{
	if (!b)return;
	projectiles.push_back(std::move(b));
}

/*弾の更新*/
void ProjectileManager::Update()
{
	Debug::Watch("bullet", projectiles.size());
	for (int i = (int)projectiles.size() - 1; i >= 0; i--){
		projectiles[i]->Update();	//弾1つ1つを更新させる
		
		//生存タグのない弾を消去する
		if (!projectiles[i]->IsAlive()) {
			projectiles[i] = std::move(projectiles.back());
			projectiles.pop_back();
		}
	}
}


/*描画*/
void ProjectileManager::Draw()
{
	for (const auto& p : projectiles)
	{
		p->Draw();	//弾1つ1つ描画させる
	}
}

/*全消去*/
void ProjectileManager::Clear()
{
	projectiles.clear();
}
