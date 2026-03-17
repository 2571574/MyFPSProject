#include "Time.h"
#include "DxLib.h"

Time::Time() 
    : nowTime(0)
    , oldTime(0)
    , deltaTime(0.0f) {
}

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

    //秒単位で取得
    deltaTime = (nowTime - oldTime) / 1000.0f;
    //最大0.1秒に制限
    if (deltaTime > 0.1f) {
        deltaTime = 0.1f;
    }
    oldTime = nowTime;
}