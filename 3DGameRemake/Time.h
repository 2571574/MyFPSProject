#pragma once
/*時間を管理するクラス*/
class Time
{
private:
	int nowTime;	//今
	int oldTime;	//1フレーム前
	float deltaTime;//経過時間
	Time();			//コンストラクタ
public:
	static Time& GetIns();	//インスタンスを取得
	void Update();			//更新
	//経過時間を得る
	float GetDelta() { return deltaTime; };
};
