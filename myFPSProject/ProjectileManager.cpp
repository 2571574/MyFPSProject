#include "ProjectileManager.h"
#include"Debug.h"

#include <algorithm>

ProjectileManager& ProjectileManager::GetIns()
{
	static ProjectileManager ins;
	return ins;
}


void ProjectileManager::Spawn(std::unique_ptr<BaseProjectile> b)
{
	if (!b)return;
	projectiles.push_back(std::move(b));
}

void ProjectileManager::Update()
{
	for (int i = (int)projectiles.size() - 1; i >= 0; i--){
		projectiles[i]->Update();	
	}

	//生存タグのない弾を消去する
	std::erase_if(projectiles, [](const auto& p) {
		return !p->GetAlive();
	});
}

void ProjectileManager::Draw()
{
	for (const auto& p : projectiles)
	{
		p->Draw();
	}
}

void ProjectileManager::Clear()
{
	projectiles.clear();
}
