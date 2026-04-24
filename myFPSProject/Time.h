#pragma once
/// <summary>
/// 時間を管理するクラス
/// </summary>
class Time
{
private:
	int nowTime;	//今
	int oldTime;	//1フレーム前
	float deltaTime;//経過時間
	Time();			
public:
	static Time& GetIns();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 前のフレームからの経過時間を得る
	/// </summary>
	/// <returns>秒単位の経過時間</returns>
	float GetDelta() { return deltaTime; };
};
