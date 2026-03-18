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
} 

void TutorialScene::Init() {
	currentPhase = TutorialPhase::MOVEMENT;
	currentEnemyInfo = -1;

	stageHandle = ResourceManager::GetIns().GetModel("Resource/TutorialArena.mv1");

	//ステージセット
	MV1SetPosition(stageHandle, VGet(0.0f, 0.0f, 0.0f));
	MV1SetScale(stageHandle, VGet(0.02f, 0.02f, 0.02f));
	MV1SetupCollInfo(stageHandle, -1, 8, 8, 8);

	player.SetStageHandle(stageHandle);
	CollisionManager::GetIns().SetStageHandle(stageHandle);
	EnemyManager::GetIns().NoPathInit(stageHandle,&player);
	ItemManager::GetIns().SetStageHandle(stageHandle);

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
}

void TutorialScene::Update() {
	float dt = Time::GetIns().GetDelta();

	//ポーズ処理
	if (InputManager::GetIns().IsActionTrigger(ActionID::PAUSE)) {
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
		pauseSelectNum--;
		if (pauseSelectNum < 0) pauseSelectNum = PAUSE_MAX - 1;
	}
	if (InputManager::GetIns().IsActionTrigger(ActionID::MENU_DOWN)) {
		pauseSelectNum++;
		if (pauseSelectNum >= PAUSE_MAX) pauseSelectNum = 0;
	}
	if (InputManager::GetIns().IsActionTrigger(ActionID::MENU_SELECT)) {
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
						DrawFormatString((int)sp.x - 40, (int)sp.y, GetColor(0, 0, 0), "Spawn: %s", name);
					}
				}
			}
		}
	}

	for (auto& t : target) if (t) t->Draw();
	EnemyManager::GetIns().Draw();

	ProjectileManager::GetIns().Draw();
	ItemManager::GetIns().Draw();
	player.Draw();

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
	DrawString(20, 20, "- チュートリアル -", colorTitle);

	switch (currentPhase) {
	case TutorialPhase::MOVEMENT:
		DrawString(TEXT_X, TEXT_Y, "[Movement]", colorTitle);
		DrawString(TEXT_X, TEXT_Y + LINE, "移動 : WASD", colorText);
		DrawString(TEXT_X, TEXT_Y + LINE * 2, "走る : Shift", colorText);
		DrawString(TEXT_X, TEXT_Y + LINE * 3, "ジャンプ : Space", colorText);
		DrawString(TEXT_X, TEXT_Y + LINE * 4, "しゃがむ : Control", colorText);
		DrawString(TEXT_X, TEXT_Y + LINE * 5, "一定速度でしゃがむとスライディング", colorText);
		break;

	case TutorialPhase::COMBAT:
		DrawString(TEXT_X, TEXT_Y, "[Combat]", colorTitle);
		DrawString(TEXT_X, TEXT_Y + LINE, "射撃 : 左クリック", colorText);
		DrawString(TEXT_X, TEXT_Y + LINE * 2, "狙う : 右クリック", colorText);
		DrawString(TEXT_X, TEXT_Y + LINE * 3, "的を撃つと対応する敵がスポーン", colorText);

		// スポーンした敵の情報
		if (currentEnemyInfo == 0) {
			DrawString(TEXT_X, TEXT_Y + LINE * 5, "< 近接 >", colorWarning);
			DrawString(TEXT_X, TEXT_Y + LINE * 6, "普通の敵", colorText);
			DrawString(TEXT_X, TEXT_Y + LINE * 7, "まっすぐ追いかけて攻撃してきます。", colorText);
		}
		else if (currentEnemyInfo == 1) {
			DrawString(TEXT_X, TEXT_Y + LINE * 5, "< ライフル >", colorWarning);
			DrawString(TEXT_X, TEXT_Y + LINE * 6, "中距離を保って弾を撃ってきます。", colorText);
		}
		else if (currentEnemyInfo == 2) {
			DrawString(TEXT_X, TEXT_Y + LINE * 5, "< スナイパー >", colorWarning);
			DrawString(TEXT_X, TEXT_Y + LINE * 6, "長距離から撃ってきます。レーザーに注意", colorText);
		}
		else if (currentEnemyInfo == 3) {
			DrawString(TEXT_X, TEXT_Y + LINE * 5, "< 爆弾 >", colorWarning);
			DrawString(TEXT_X, TEXT_Y + LINE * 6, "近づいて少し経つと爆発します。", colorText);
		}
		break;

	case TutorialPhase::FREERANGE:
		DrawString(TEXT_X, TEXT_Y, "[Free Range]", colorTitle);
		DrawString(TEXT_X, TEXT_Y + LINE, "武器を拾う : F", colorText);
		DrawString(TEXT_X, TEXT_Y + LINE * 2, "武器変更 : マウスホイール", colorText);
		DrawString(TEXT_X, TEXT_Y + LINE * 3, "自由に射撃できます。", colorText);
		DrawString(TEXT_X, TEXT_Y + LINE * 4, "戻るときはメニュー(esc)から", colorText);
		DrawString(TEXT_X, TEXT_Y + LINE * 5, "タイトルに戻ってください。", colorText);
		// 武器情報HUD（持っている武器に応じて変化）
		if (Weapon* w = player.GetWeapon()) {
			auto id = w->GetSpec().id;
			DrawString(TEXT_X, TEXT_Y + LINE * 7, "現在の武器:", colorInfo);

			switch (id) {
				case WeaponID::PIS:
				DrawString(TEXT_X, TEXT_Y + LINE * 8, "ハンドガン", colorWarning); 
				DrawString(TEXT_X, TEXT_Y + LINE * 9, "デフォルト武器。　弾が無限", colorText); break;
				case WeaponID::AR:
				DrawString(TEXT_X, TEXT_Y + LINE * 8, "アサルトライフル", colorWarning);
				DrawString(TEXT_X, TEXT_Y + LINE * 9, "火力、弾速、連射速度全て標準。", colorText); break;
			case WeaponID::SR:
				DrawString(TEXT_X, TEXT_Y + LINE * 8, "スナイパーライフル", colorWarning);
				DrawString(TEXT_X, TEXT_Y + LINE * 9, "単発高火力。弱点ヒットで1撃", colorText); break;
			case WeaponID::SMG:
				DrawString(TEXT_X, TEXT_Y + LINE * 8, "サブマシンガン", colorWarning);
				DrawString(TEXT_X, TEXT_Y + LINE * 9, "近距離最強。遠距離は苦手", colorText); break;
			case WeaponID::LR:
				DrawString(TEXT_X, TEXT_Y + LINE * 8, "ロケットランチャー", colorWarning);
				DrawString(TEXT_X, TEXT_Y + LINE * 9, "範囲高火力武器。自爆に注意", colorText); break;
			}
		}
	}

	if (isPaused) PauseDraw();
}

void TutorialScene::PauseDraw() {
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
	DrawBox(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, GetColor(0, 0, 0), TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	DrawString(CENTER_X - 40, CENTER_Y - 100, "PAUSE", GetColor(0, 0, 0));
	int colorResume = (pauseSelectNum == RESUME) ? GetColor(255, 255, 0) : GetColor(0, 0, 0);
	int colorTitle = (pauseSelectNum == RETURN_TITLE) ? GetColor(255, 255, 0) : GetColor(0, 0, 0);
	DrawString(CENTER_X - 80, CENTER_Y + 40 * pauseSelectNum, ">", GetColor(0, 0, 0));
	DrawString(CENTER_X - 60, CENTER_Y, "Resume", colorResume);
	DrawString(CENTER_X - 60, CENTER_Y + 40, "Return Title", colorTitle);
}