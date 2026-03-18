#include "TextManager.h"
TextManager::TextManager() {
	weaponNames = {
		{WeaponID::AR,"アサルトライフル"},
		{WeaponID::SR,"スナイパーライフル"},
		{WeaponID::LR, "ロケットランチャー"},
		{WeaponID::SMG,"サブマシンガン"},
		{WeaponID::PIS, "ハンドガン"},
	};
	
	actionNames = {
		{ActionID::MOVE_FORWARD, "前進"},
		{ActionID::MOVE_LEFT,"左"},
		{ActionID::MOVE_BACK,"後退"},
		{ActionID::MOVE_RIGHT,"右"},
		{ActionID::FIRE,"射撃"},
		{ActionID::ADS,"狙う"},
		{ActionID::RELOAD, "リロード"},
		{ActionID::RUN,"ダッシュ"},
		{ActionID::JUMP,"ジャンプ"},
		{ActionID::CROUCH,"しゃがむ"},
		{ActionID::WEAPON_NEXT,"次の武器"},
		{ActionID::WEAPON_PREV, "前の武器"},
		{ActionID::INTERACT,"拾う"}
	};
	deathCause = {
		{WeaponID::ENEMY_KNIFE, "死因:格闘"},
		{WeaponID::ENEMY_AR,"死因:ライフル"},
		{WeaponID::ENEMY_SR,"死因:スナイパー"},
		{WeaponID::ENEMY_EXPLOSION,"死因:爆発"},
		{WeaponID::LR,"死因:自爆"}
	};

	menuDescription = {
		{0,"ゲームを開始します。"},
		{1,"設定を変更します。"},
		{2,"クレジットを表示します。"},
		{3,"ゲームを終了します。"},
		{4,"BGMの音量を変更します。"},
		{5,"SEの音量を変更します。"},
		{6,"マウスの感度を変更します。"},
		{7,"コントローラーの感度を変更します。"},
		{8,"視野角を変更します。"},
		{9,"移動時の画面の揺れを変更します。"},
		{10,"銃の反動がある程度戻るようになります。"},
		{11,"キーバインドを編集します。"},
		{12,""},
		{13,"チュートリアル"},
		{14," イージー\n 最初から武器を持ち、弾が無限になります。"},
		{15," ノーマル\n 拾った武器で戦います。"},
		{16," ハード\n 持てる武器が１つまでになります。"},
	};
}

TextManager& TextManager::GetIns() {
	static TextManager ins;
	return ins;
}

const char* TextManager::GetWeaponName(WeaponID id)const {
	auto it = weaponNames.find(id);
	if (it != weaponNames.end()) {
		return it->second.c_str();
	}
	return "Unknown";
}

const char* TextManager::GetActionName(ActionID id)const {
	auto it = actionNames.find(id);
	if (it != actionNames.end()) {
		return it->second.c_str();
	}
	return "Unknown";
}

const char* TextManager::GetCauseName(WeaponID id)const {
	auto it = deathCause.find(id);
	if (it != deathCause.end()) {
		return it->second.c_str();
	}
	return "Unknown";
}

const char* TextManager::GetMenuDescription(int id)const {
	auto it = menuDescription.find(id);
	if (it != menuDescription.end()) {
		return it->second.c_str();
	}
	return "";
}