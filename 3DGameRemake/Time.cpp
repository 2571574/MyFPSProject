#include "Time.h"
#include "DxLib.h"
/*コンストラクタ*/
Time::Time() : nowTime(0), oldTime(0), deltaTime(0.0f) {}

/*インスタンスを取得*/
Time& Time::GetIns() {
    static Time instance;
    return instance;
}

/*更新*/
void Time::Update() {
    nowTime = GetNowCount();
    if (oldTime == 0) {
        oldTime = nowTime;
    }
    deltaTime = (nowTime - oldTime) / 1000.0f;
    oldTime = nowTime;
}