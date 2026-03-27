#include "Time.h"
#include "DxLib.h"
#include "Param/System.h"

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
    deltaTime = (nowTime - oldTime) * Global::Math::MS_TO_SEC;

    //最大0.1秒に制限
    if (deltaTime > System::Time::MAX_DELTA_TIME) {
        deltaTime = System::Time::MAX_DELTA_TIME;
    }
    oldTime = nowTime;
}