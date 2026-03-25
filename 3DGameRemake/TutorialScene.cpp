#include "TutorialScene.h"
#include "TitleScene.h"
#include "InputManager.h"
#include "Time.h"
#include "CollisionManager.h"
#include "ItemManager.h"
#include "EnemyManager.h"
#include "ProjectileManager.h"
#include "MeleeEnemy.h"
#include "RifleEnemy.h"
#include "SniperEnemy.h"
#include "RollingEnemy.h"
#include "ResourceManager.h"
#include "TextManager.h"
#include "EffectManager.h"
#include "SoundManager.h"

namespace {
	constexpr float TUTORIAL_SPAWN_TIMER = 0.5f;
	constexpr int DIALOG_X2 = 350;
	constexpr int DIALOG_Y = 250;
}
TutorialScene::TutorialScene(SceneManager* manager)
	:BaseScene(manager)
	, player(VGet(20.0f, 0.0f, 28.0f), &camera, PlayMode::MODE_TUTORIAL)
	, stageHandle(-1)
	, isPaused(false)
	, pauseSelectNum(0) {
}

TutorialScene::~TutorialScene() {
	EnemyManager::GetIns().Clear();
	ProjectileManager::GetIns().Clear();
	ItemManager::GetIns().Clear();
	EffectManager::GetIns().Clear();
} 

void TutorialScene::Init() {
	currentPhase = TutorialPhase::MOVEMENT;
	currentEnemyInfo = -1;

	stageHandle = ResourceManager::GetIns().GetModel("Resource/TutorialArena.mv1");
	EffectManager::GetIns().Clear();
	//ステージセット
	MV1SetPosition(stageHandle, VGet(0.0f, 0.0f, 0.0f));
	MV1SetScale(stageHandle, VGet(0.02f, 0.02f, 0.02f));
	MV1SetupCollInfo(stageHandle, -1, 8, 8, 8);

	player.SetStageHandle(stageHandle);
	CollisionManager::GetIns().SetStageHandle(stageHandle);
	EnemyManager::GetIns().NoPathInit(stageHandle,&player);
	ItemManager::GetIns().SetStageHandle(stageHandle);

	fontLarge = ResourceManager::GetIns().GetFont("Resource/Font/JetBrainsMono_36.dft");
	fontMedium = ResourceManager::GetIns().GetFont("Resource/Font/NotoSansJP_20.dft");
	fontSmall = ResourceManager::GetIns().GetFont("Resource/Font/NotoSansJP_16.dft");

	//武器スポナー
	std::vector<SpawnerSetup>spawnerSetups = {
		{VGet(-15.0f,0.4f,-16.0f),WeaponID::AR,TUTORIAL_SPAWN_TIMER},
		{VGet(-10.0f,0.4f,-16.0f),WeaponID::SR,TUTORIAL_SPAWN_TIMER},
		{VGet(0.0f,0.4f,-16.0f),WeaponID::SMG,TUTORIAL_SPAWN_TIMER},
		{VGet(5.0f,0.4f,-16.0f),WeaponID::LR,TUTORIAL_SPAWN_TIMER}
	};
	ItemManager::GetIns().InitSpawners(spawnerSetups);

	//的ダミー
	for (int i = 0; i < 3; ++i) {
		auto Target = std::make_unique<Dummy>(VGet(-26.0f + i * 15.0f, 0.0f, -5.0f + i * 15.0f), &player, true);
		target.push_back(std::move(Target));
	}

	//ボタンダミー 
	for (int i = 0; i < 4; i++) {
		SpawnButton btn;
		btn.pos = VGet(28.0f + i * -5.0f, 0.0f, -28.0f);
		btn.dummy = std::make_unique<Dummy>(btn.pos,&player,false);
		btn.enemyType = i;
		btn.spawnCT = 0.0f;
		button.push_back(std::move(btn));
	}

	SoundManager::GetIns().PlayBGM("Resource/Sound/GameBGM.wav");
}

void TutorialScene::Update() {
	Debug::Update();
	float dt = Time::GetIns().GetDelta();

	//ポーズ処理
	if (InputManager::GetIns().IsActionTrigger(ActionID::PAUSE)) {
		SoundManager::GetIns().PlaySE("Resource/Sound/pause.ogg");
		isPaused = !isPaused;
		pauseSelectNum = 0;
	}
	if (isPaused) {
		PauseUpdate();
		return;
	}

	player.Update();

	//ダミー更新処理
	for (auto& btn : button) {
		if (btn.spawnCT > 0.0f)
			btn.spawnCT -= dt;
		if (btn.dummy)btn.dummy->Update();
	}
	for (auto& t : target) {
		if (t) {
			t->Update();
			if (t->GetHP() <= 0) t->revive();
		}
	}


	VECTOR pPos = player.GetPos();

	//ステート更新箇所の座標
	bool isPlayerCombatArea = (pPos.x >= -22.0f && pPos.x <= 9.0f && pPos.z >= -30.0f && pPos.z <= -20.0f);
	bool MovementToCombat = (pPos.x >= 11.0f && pPos.x <= 30.0f && pPos.z < -20.0f);
	bool CombatToMovement = (pPos.x >= 11.0f && pPos.x <= 30.0f && pPos.z > -18.0f);
	bool CombatToFreerange = (pPos.x >= -30.0f && pPos.x <= -24.0f && pPos.z > -18.0f);
	bool FreerangeToCombat = (pPos.x >= -30.0f && pPos.x <= -24.0f && pPos.z < -20.0f);


	if (isPlayerCombatArea) {
		EnemyManager::GetIns().Update();
	}
	EffectManager::GetIns().Update();
	CollisionManager::GetIns().Update();
	ProjectileManager::GetIns().Update();
	ItemManager::GetIns().Update(&player);
	if (player.GetHP() <= 0) {
		player.revive();
	}
	
	switch (currentPhase) {
	case TutorialPhase::MOVEMENT:
		if (MovementToCombat) {
			currentPhase = TutorialPhase::COMBAT;
			EnemyManager::GetIns().Clear();
			currentEnemyInfo = -1;
		}
		break;
	case TutorialPhase::COMBAT: {
		static bool entered = false;
		if (isPlayerCombatArea) {
			entered = true;
		}
		if (entered && !isPlayerCombatArea) {
			EnemyManager::GetIns().Clear();
			entered = false;
		}
		if (CombatToMovement) {
			currentPhase = TutorialPhase::MOVEMENT;
			EnemyManager::GetIns().Clear();
			currentEnemyInfo = -1;
		}
		if (CombatToFreerange) {
			currentPhase = TutorialPhase::FREERANGE;
			EnemyManager::GetIns().Clear();
			currentEnemyInfo = -1;
		}

		//スポーンボタンの処理
		for (auto& btn : button) {
			if (btn.dummy && btn.dummy->GetHP() < btn.dummy->GetStatus().maxHP) {
				btn.dummy->revive();

				if (btn.spawnCT <= 0.0f) {

					EnemyManager::GetIns().Clear();

					currentEnemyInfo = btn.enemyType;
					VECTOR spawnPos = VGet(-18, 1, -24);

					if (btn.enemyType == 0)EnemyManager::GetIns().Spawn(std::make_unique<MeleeEnemy>(spawnPos, &player));
					if (btn.enemyType == 1)EnemyManager::GetIns().Spawn(std::make_unique<RifleEnemy>(spawnPos, &player));
					if (btn.enemyType == 2)EnemyManager::GetIns().Spawn(std::make_unique<SniperEnemy>(spawnPos, &player));
					if (btn.enemyType == 3)EnemyManager::GetIns().Spawn(std::make_unique<RollingEnemy>(spawnPos, &player));
					btn.spawnCT = 1.0f;
				}
			}
		}
		break;
	}
	case TutorialPhase::FREERANGE:
		if (FreerangeToCombat) {
			currentPhase = TutorialPhase::COMBAT;
			EnemyManager::GetIns().Clear();
			currentEnemyInfo = -1;
		}
		break;
	}
}

void TutorialScene::PauseUpdate() {
	SetMousePoint(CENTER_X, CENTER_Y);
	if (InputManager::GetIns().IsActionTrigger(ActionID::MENU_UP)) {
		SoundManager::GetIns().PlaySE("Resource/Sound/cursormove.ogg");
		pauseSelectNum--;
		if (pauseSelectNum < 0) pauseSelectNum = PAUSE_MAX - 1;
	}
	if (InputManager::GetIns().IsActionTrigger(ActionID::MENU_DOWN)) {
		SoundManager::GetIns().PlaySE("Resource/Sound/cursormove.ogg");
		pauseSelectNum++;
		if (pauseSelectNum >= PAUSE_MAX) pauseSelectNum = 0;
	}
	if (InputManager::GetIns().IsActionTrigger(ActionID::MENU_SELECT)) {
		SoundManager::GetIns().PlaySE("Resource/Sound/select.ogg");
		if (pauseSelectNum == RESUME) isPaused = false;
		else if (pauseSelectNum == RETURN_TITLE) manager->ChangeScene(std::make_unique<TitleScene>(manager));
	}
}


void TutorialScene::Draw() {
	if (stageHandle != -1) MV1DrawModel(stageHandle);
	ItemManager::GetIns().SetCamPos(camera.GetPos());
	// ボタン（ダミー）の描画とテキスト
	for (auto& btn : button) {
		if (btn.dummy) {
			btn.dummy->Draw();	
			VECTOR camPos = camera.GetPos(); 
			float distance = VSize(VSub(camPos, btn.pos));
			if (distance <= 15.0f){
				VECTOR textPos3D = VAdd(btn.pos, VGet(0.0f, 2.0f, 0.0f));
				VECTOR sp = ConvWorldPosToScreenPos(textPos3D);
				if (sp.z >= 0 && sp.z <= 1.0f) {
					MV1_COLL_RESULT_POLY hit = MV1CollCheck_Line(stageHandle, -1, camPos, textPos3D);
					if (hit.HitFlag == 0) {
						const char* name = "";
						if (btn.enemyType == 0) name = "Melee";
						else if (btn.enemyType == 1) name = "Rifle";
						else if (btn.enemyType == 2) name = "Sniper";
						else if (btn.enemyType == 3) name = "Rolling";
						float fadeStartDist = 10.0f;
						float alphaRate = 1.0f;
						if (distance > fadeStartDist) {
							alphaRate = 1.0f - ((distance - fadeStartDist) / 5.0f);
						}
						int alpha = static_cast<int>(255 * alphaRate);

						::SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
						::DrawFormatStringToHandle(static_cast<int>(sp.x) - 40, static_cast<int>(sp.y), GetColor(255, 255, 255), fontMedium, "Spawn: %s", name);
						::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
					}
				}
			}
		}
	}

	for (auto& t : target) if (t) t->Draw();
	EnemyManager::GetIns().Draw();
	EffectManager::GetIns().Draw();

	ProjectileManager::GetIns().Draw();
	ItemManager::GetIns().Draw();
	player.Draw();
	Debug::Draw();

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 100);
	DrawBox(0, CENTER_Y - DIALOG_Y, DIALOG_X2, CENTER_Y + DIALOG_Y, GetColor(0, 0, 0),true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	// --- HUD描画 ---
	const int colorTitle = GetColor(0, 255, 0);
	const int colorText = GetColor(255, 255, 255);
	const int colorWarning = GetColor(255, 100, 100);
	const int colorAlert = GetColor(255, 255, 0);
	const int colorInfo = GetColor(255, 200, 0);

	const int TEXT_X = 20;
	const int TEXT_Y = 300;
	const int LINE = 30;


	std::string moveUpKey = TextManager::GetIns().GetActionKeyString(ActionID::MOVE_FORWARD);
	std::string runKey = TextManager::GetIns().GetActionKeyString(ActionID::RUN);
	std::string jumpKey = TextManager::GetIns().GetActionKeyString(ActionID::JUMP);
	std::string crouchKey = TextManager::GetIns().GetActionKeyString(ActionID::CROUCH);
	std::string fireKey = TextManager::GetIns().GetActionKeyString(ActionID::FIRE);
	std::string adsKey = TextManager::GetIns().GetActionKeyString(ActionID::ADS);
	std::string reloadKey = TextManager::GetIns().GetActionKeyString(ActionID::RELOAD);
	std::string interactKey = TextManager::GetIns().GetActionKeyString(ActionID::INTERACT);
	std::string nextWepKey = TextManager::GetIns().GetActionKeyString(ActionID::WEAPON_NEXT);
	std::string pauseKey = TextManager::GetIns().GetActionKeyString(ActionID::PAUSE);


	switch (currentPhase) {
	case TutorialPhase::MOVEMENT:
		DrawStringToHandle(TEXT_X, TEXT_Y, "[MOVEMENT]", colorTitle, fontLarge);
		DrawStringToHandle(TEXT_X, TEXT_Y + LINE * 2, ("移動 : " + moveUpKey + "等").c_str(), colorText, fontSmall);
		DrawStringToHandle(TEXT_X, TEXT_Y + LINE * 3, ("走る : " + runKey).c_str(), colorText, fontSmall);
		DrawStringToHandle(TEXT_X, TEXT_Y + LINE * 4, ("ジャンプ : " + jumpKey).c_str(), colorText, fontSmall);
		DrawStringToHandle(TEXT_X, TEXT_Y + LINE * 5, ("しゃがむ : " + crouchKey).c_str(), colorText, fontSmall);
		DrawStringToHandle(TEXT_X, TEXT_Y + LINE * 6, "一定速度でしゃがむとスライディング", colorText, fontSmall);
		break;

	case TutorialPhase::COMBAT:
		DrawStringToHandle(TEXT_X, TEXT_Y, "[COMBAT]", colorTitle, fontLarge);
		DrawStringToHandle(TEXT_X, TEXT_Y + LINE * 2, ("射撃 : " + fireKey).c_str(), colorText, fontSmall);
		DrawStringToHandle(TEXT_X, TEXT_Y + LINE * 3, ("狙う : " + adsKey).c_str(), colorText, fontSmall);
		DrawStringToHandle(TEXT_X, TEXT_Y + LINE * 4, ("リロード : " + reloadKey).c_str(), colorText, fontSmall);
		DrawStringToHandle(TEXT_X, TEXT_Y + LINE * 6, "しゃがむことで反動と拡散が軽減される", colorText, fontSmall);
		DrawStringToHandle(TEXT_X, TEXT_Y + LINE * 7, "ヘッドショットするとダメージは２倍", colorText, fontSmall);
		DrawStringToHandle(TEXT_X, TEXT_Y + LINE * 9, "的を撃つと対応する敵がスポーン", colorText, fontSmall);

		// スポーンした敵の情報
		if (currentEnemyInfo == 0) {
			DrawStringToHandle(TEXT_X, TEXT_Y + LINE * 10, "< 近接 >", colorWarning, fontSmall);
			DrawStringToHandle(TEXT_X, TEXT_Y + LINE * 11, "まっすぐ追いかけて攻撃してきます。", colorText, fontSmall);
			DrawStringToHandle(TEXT_X, TEXT_Y + LINE * 12, "至って普通の敵です。", colorText, fontSmall);
		}
		else if (currentEnemyInfo == 1) {
			DrawStringToHandle(TEXT_X, TEXT_Y + LINE * 10, "< ライフル >", colorWarning, fontSmall);
			DrawStringToHandle(TEXT_X, TEXT_Y + LINE * 11, "中距離を保って弾を撃ってきます。", colorText, fontSmall);
			DrawStringToHandle(TEXT_X, TEXT_Y + LINE * 12, "１発のダメージは少なめ", colorText, fontSmall);
		}
		else if (currentEnemyInfo == 2) {
			DrawStringToHandle(TEXT_X, TEXT_Y + LINE * 10, "< スナイパー >", colorWarning, fontSmall);
			DrawStringToHandle(TEXT_X, TEXT_Y + LINE * 11, "長距離から撃ってきます。", colorText, fontSmall);
			DrawStringToHandle(TEXT_X, TEXT_Y + LINE * 12, "レーザーに注意。", colorText, fontSmall);
		}
		else if (currentEnemyInfo == 3) {
			DrawStringToHandle(TEXT_X, TEXT_Y + LINE * 10, "< 爆弾 >", colorWarning, fontSmall);
			DrawStringToHandle(TEXT_X, TEXT_Y + LINE * 11, "近づいて少し経つと爆発します。", colorText, fontSmall);
			DrawStringToHandle(TEXT_X, TEXT_Y + LINE * 12, "倒した時にも爆発するので注意。", colorText, fontSmall);
		}
		break;

	case TutorialPhase::FREERANGE:
		DrawStringToHandle(TEXT_X, TEXT_Y, "[FREE RANGE]", colorTitle, fontLarge);
		DrawStringToHandle(TEXT_X, TEXT_Y + LINE * 2, ("武器を拾う : " + interactKey).c_str(), colorText, fontSmall);
		DrawStringToHandle(TEXT_X, TEXT_Y + LINE * 3, ("武器切替 : " + nextWepKey + "等").c_str(), colorText, fontSmall);
		DrawStringToHandle(TEXT_X, TEXT_Y + LINE * 4, "※所持上限時は手元の武器と交換します", colorAlert, fontSmall);
		DrawStringToHandle(TEXT_X, TEXT_Y + LINE * 5, "自由に射撃できます。", colorText, fontSmall);
		DrawStringToHandle(TEXT_X, TEXT_Y + LINE * 14, ("戻るときはメニュー(" + pauseKey + ")から").c_str(), colorText, fontSmall);
		DrawStringToHandle(TEXT_X, TEXT_Y + LINE * 15, "タイトルに戻ってください。", colorText, fontSmall);


		if (Weapon* w = player.GetWeapon()) {
			auto id = w->GetSpec().id;
			DrawStringToHandle(TEXT_X, TEXT_Y + LINE * 7, "現在の武器", colorInfo, fontSmall);

			switch (id) {
			case WeaponID::PIS:
				DrawStringToHandle(TEXT_X, TEXT_Y + LINE * 8, "<ハンドガン>", colorWarning, fontSmall);
				DrawStringToHandle(TEXT_X, TEXT_Y + LINE * 9, "デフォルト武器。　弾が無限", colorText, fontSmall); break;
			case WeaponID::AR:
				DrawStringToHandle(TEXT_X, TEXT_Y + LINE * 8, "<アサルトライフル>", colorWarning, fontSmall);
				DrawStringToHandle(TEXT_X, TEXT_Y + LINE * 9, "火力、弾速、連射速度全て標準。", colorText, fontSmall); break;
			case WeaponID::SR:
				DrawStringToHandle(TEXT_X, TEXT_Y + LINE * 8, "<スナイパーライフル>", colorWarning, fontSmall);
				DrawStringToHandle(TEXT_X, TEXT_Y + LINE * 9, "単発高火力。弱点ヒットで1撃", colorText, fontSmall); break;
			case WeaponID::SMG:
				DrawStringToHandle(TEXT_X, TEXT_Y + LINE * 8, "<サブマシンガン>", colorWarning, fontSmall);
				DrawStringToHandle(TEXT_X, TEXT_Y + LINE * 9, "近距離最強。遠距離は苦手", colorText, fontSmall); break;
			case WeaponID::LR:
				DrawStringToHandle(TEXT_X, TEXT_Y + LINE * 8, "<ロケットランチャー>", colorWarning, fontSmall);
				DrawStringToHandle(TEXT_X, TEXT_Y + LINE * 9, "範囲高火力武器。自爆に注意", colorText, fontSmall); break;
			}
		}
	}

	if (isPaused) PauseDraw();
}

void TutorialScene::PauseDraw() {
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
	DrawBox(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, GetColor(0, 0, 0), TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	DrawStringToHandle(CENTER_X - 40, CENTER_Y - 100, "PAUSE", GetColor(255, 255, 255), fontLarge);
	int colorResume = (pauseSelectNum == RESUME) ? GetColor(255, 255, 0) : GetColor(255, 255, 255);
	int colorTitle = (pauseSelectNum == RETURN_TITLE) ? GetColor(255, 255, 0) : GetColor(255, 255, 255);
	DrawStringToHandle(CENTER_X - 80, CENTER_Y + 60 * pauseSelectNum, ">", GetColor(255, 255, 0), fontMedium);
	DrawStringToHandle(CENTER_X - 60, CENTER_Y, "Resume", colorResume, fontMedium);
	DrawStringToHandle(CENTER_X - 60, CENTER_Y + 60, "Return Title", colorTitle, fontMedium);
}