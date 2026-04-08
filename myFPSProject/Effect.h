#pragma once
#include "DxLib.h"
#include "Time.h"

#include <vector>

/// <summary>
///	エフェクトの基底クラス
/// </summary>
class Effect
{
protected:
	VECTOR position;	//座標
	float lifeTime;		//描画時間
	float maxLifeTime;	//最大描画時間
	bool alive;			//生存タグ

public:
	Effect(VECTOR pos, float life) : position(pos), lifeTime(life), maxLifeTime(life), alive(true) {}
	virtual ~Effect() = default;

	virtual void Update() = 0;
	virtual void Draw() = 0;
	bool IsAlive() const { return alive; }
};

/// <summary>
/// 敵の死亡時エフェクト
/// </summary>
class DeathEffect : public Effect {
private:
	VECTOR velocity;	//飛散速度
	float size;			//大きさ
	int color;			//色
	float floorY;		//床の高さ

	VECTOR rotAxis;		//回転ベクトル
	float angle;		//角度
	float rotSpeed;		//回転速度

public:
	DeathEffect(VECTOR pos, VECTOR vel, float life, float s, float fY, int c);
	void Update() override;
	void Draw() override;
};

/// <summary>
/// 銃のマズルフラッシュのエフェクト
/// </summary>
class MuzzleFlashEffect : public Effect {
private:
	VECTOR direction;	//方向
	float size;			//大きさ
	int color;			//色
public:
	MuzzleFlashEffect(VECTOR pos, VECTOR dir, float life, float s, int c);
	void Update() override;
	void Draw() override;
};

/// <summary>
/// 着弾エフェクト
/// </summary>
class HitEffect : public Effect {
private:
	VECTOR velocity;	//速度
	float size;			//大きさ
	int color;			//色
	float floorY;		//床の高さ
public:
	HitEffect(VECTOR pos, VECTOR vel, float life, float s, float fY, int c);
	void Update() override;
	void Draw() override;
};

/// <summary>
/// スポーンエフェクト
/// </summary>
class SpawnEffect : public Effect {
private:
	float targetHeight;		//ターゲットの高さ
	float targetRadius;		//ターゲットの半径

	//エフェクトの基本情報
	struct Particle {
		VECTOR pos;	//座標
		VECTOR vel;	//速度
		float life;	//現在描画時間
		float maxLife;	//指定描画時間
		float size;	//大きさ
		int color;	//色
	};
	std::vector<Particle>particles;	//パーティクルの配列
public:
	SpawnEffect(VECTOR pos, float height, float radius, float life);
	void Update() override;
	void Draw() override;
};

/// <summary>
/// 爆発エフェクト
/// </summary>
class ExplosionEffect : public Effect {
private:
	float maxRadius;	//最大半径
	int color;			//色

	//飛散する破片
	struct Debris {
		VECTOR pos;		//座標
		VECTOR vel;		//速度
		VECTOR rotAxis;	//方向ベクトル
		float angle;	//角度
		float rotSpeed;	//回転速度
		float size;		//大きさ
	};
	std::vector<Debris> debrisList;		//破片の配列
public:
	ExplosionEffect(VECTOR pos, float radius, int c, float life);
	void Update() override;
	void Draw() override;
};

/// <summary>
/// ヒットスキャンの弾のトレイル
/// </summary>
class HitScanTrail : public Effect {
private:
	VECTOR startPos;	//始点
	VECTOR endPos;		//終点
	int color;			//色
	float initialRad;	//半径
public:
	HitScanTrail(VECTOR start, VECTOR end, int c, float life);
	void Update() override;
	void Draw() override;
};

//プロジェクタイルの弾のトレイル
class ProjectileTrailEffect : public Effect {
private:
	VECTOR startPos;		//始点
	VECTOR endPos;			//終点
	int color;				//色
	float initialRadius;	//半径

public:
	ProjectileTrailEffect(VECTOR start, VECTOR end, float radius, int c, float life);
	void Update() override;
	void Draw() override;
};