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
#include "Param/Global.h"
#include "Param/Scene.h"
#include "Param/System.h"
#include "Param/Visual.h"

TutorialScene::TutorialScene(SceneManager* manager)
	:BaseScene(manager)
	, player(VGet(20.0f, 0.0f, 28.0f), &camera, PlayMode::MODE_TUTORIAL)
	, stageHandle(-1)
	, isPaused(false)
	, pauseSelectNum(0) {
	SetMousePoint(System::Window::CENTER_X, System::Window::CENTER_Y);
}

TutorialScene::~TutorialScene() {
	EnemyManager::GetIns().Clear();
	ProjectileManager::GetIns().Clear();
	ItemManager::GetIns().Clear();
	EffectManager::GetIns().Clear();
}

void TutorialScene::Init() {
	camera.SetAngle(System::Camera::DEFAULT_CAM_X, 0.0f);
	player.SyncCamAngle();

	currentPhase = TutorialPhase::MOVEMENT;
	currentEnemyInfo = -1;

	stageHandle = ResourceManager::GetIns().GetModel("Resource/TutorialArena.mv1");
	EffectManager::GetIns().Clear();

	MV1SetPosition(stageHandle, VGet(0.0f, 0.0f, 0.0f));
	MV1SetScale(stageHandle, VGet(Scene::Common::STAGE_MODEL_SCALE, Scene::Common::STAGE_MODEL_SCALE, Scene::Common::STAGE_MODEL_SCALE));
	MV1SetupCollInfo(stageHandle, -1, Scene::Common::COLLISION_SETUP_DIV_NUM, Scene::Common::COLLISION_SETUP_DIV_NUM, Scene::Common::COLLISION_SETUP_DIV_NUM);

	player.SetStageHandle(stageHandle);
	CollisionManager::GetIns().SetStageHandle(stageHandle);
	EnemyManager::GetIns().NoPathInit(stageHandle, &player);
	ItemManager::GetIns().SetStageHandle(stageHandle);

	fontLarge = ResourceManager::GetIns().GetFont("Resource/Font/JetBrainsMono_36.dft");
	fontMedium = ResourceManager::GetIns().GetFont("Resource/Font/NotoSansJP_20.dft");
	fontSmall = ResourceManager::GetIns().GetFont("Resource/Font/NotoSansJP_16.dft");

	std::vector<SpawnerSetup>spawnerSetups = {
		{VGet(-15.0f,0.4f,-16.0f),WeaponID::AR,Scene::Tutorial::TUTORIAL_SPAWN_TIMER},
		{VGet(-10.0f,0.4f,-16.0f),WeaponID::SR,Scene::Tutorial::TUTORIAL_SPAWN_TIMER},
		{VGet(0.0f,0.4f,-16.0f),WeaponID::SMG,Scene::Tutorial::TUTORIAL_SPAWN_TIMER},
		{VGet(5.0f,0.4f,-16.0f),WeaponID::LR,Scene::Tutorial::TUTORIAL_SPAWN_TIMER}
	};
	ItemManager::GetIns().InitSpawners(spawnerSetups);

	for (int i = 0; i < 3; ++i) {
		auto Target = std::make_unique<Dummy>(VGet(Scene::Tutorial::TARGET_BASE_X + i * Scene::Tutorial::TARGET_INTERVAL_X, 0.0f, Scene::Tutorial::TARGET_BASE_Z + i * Scene::Tutorial::TARGET_INTERVAL_Z), &player, true);
		target.push_back(std::move(Target));
	}

	for (int i = 0; i < 4; i++) {
		SpawnButton btn;
		btn.pos = VGet(Scene::Tutorial::BUTTON_BASE_X + i * Scene::Tutorial::BUTTON_INTERVAL_X, 0.0f, Scene::Tutorial::BUTTON_BASE_Z);
		btn.dummy = std::make_unique<Dummy>(btn.pos, &player, false);
		btn.enemyType = i;
		btn.spawnCT = 0.0f;
		button.push_back(std::move(btn));
	}

	SoundManager::GetIns().PlayBGM("Resource/Sound/GameBGM.wav");
}

void TutorialScene::Update() {
	Debug::Update();
	float dt = Time::GetIns().GetDelta();

	if (InputManager::GetIns().IsActionTrigger(ActionID::PAUSE)) {
		isPaused = !isPaused;
		pauseSelectNum = 0;

		if (isPaused) {
			SoundManager::GetIns().PauseAll();
			SoundManager::GetIns().PlaySE("Resource/Sound/pause.ogg");
		}
		else {
			SoundManager::GetIns().ResumeAll();
			SoundManager::GetIns().PlaySE("Resource/Sound/pause.ogg");
		}
	}
	if (isPaused) {
		PauseUpdate();
		return;
	}

	player.Update();

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

	bool isPlayerCombatArea = (pPos.x >= Scene::Tutorial::Boundary::COMBAT_MIN_X && pPos.x <= Scene::Tutorial::Boundary::COMBAT_MAX_X && pPos.z >= Scene::Tutorial::Boundary::COMBAT_MIN_Z && pPos.z <= Scene::Tutorial::Boundary::COMBAT_MAX_Z);
	bool MovementToCombat = (pPos.x >= Scene::Tutorial::Boundary::MOVE_TO_COMBAT_MIN_X && pPos.x <= Scene::Tutorial::Boundary::MOVE_TO_COMBAT_MAX_X && pPos.z < Scene::Tutorial::Boundary::MOVE_TO_OTHER_MIN_Z);
	bool CombatToMovement = (pPos.x >= Scene::Tutorial::Boundary::MOVE_TO_COMBAT_MIN_X && pPos.x <= Scene::Tutorial::Boundary::MOVE_TO_COMBAT_MAX_X && pPos.z > Scene::Tutorial::Boundary::MOVE_TO_OTHER_MAX_Z);
	bool CombatToFreerange = (pPos.x >= Scene::Tutorial::Boundary::COMBAT_TO_FREE_RANGE_MIN_X && pPos.x <= Scene::Tutorial::Boundary::COMBAT_TO_FREE_RANGE_MAX_X && pPos.z > Scene::Tutorial::Boundary::MOVE_TO_OTHER_MAX_Z);
	bool FreerangeToCombat = (pPos.x >= Scene::Tutorial::Boundary::COMBAT_TO_FREE_RANGE_MIN_X && pPos.x <= Scene::Tutorial::Boundary::COMBAT_TO_FREE_RANGE_MAX_X && pPos.z < Scene::Tutorial::Boundary::MOVE_TO_OTHER_MIN_Z);


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

		for (auto& btn : button) {
			if (btn.dummy && btn.dummy->GetHP() < btn.dummy->GetStatus().maxHP) {
				btn.dummy->revive();

				if (btn.spawnCT <= 0.0f) {

					EnemyManager::GetIns().Clear();

					currentEnemyInfo = btn.enemyType;
					VECTOR spawnPos = VGet(Scene::Tutorial::Pos::SPAWN_X, Scene::Tutorial::Pos::SPAWN_Y, Scene::Tutorial::Pos::SPAWN_Z);

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
	SetMousePoint(System::Window::CENTER_X, System::Window::CENTER_Y);
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
		if (pauseSelectNum == RESUME) {
			isPaused = false;
			SoundManager::GetIns().ResumeAll();
		}

		else if (pauseSelectNum == RETURN_TITLE) manager->ChangeScene(std::make_unique<TitleScene>(manager));
	}
}


void TutorialScene::Draw() {
	if (stageHandle != -1) MV1DrawModel(stageHandle);
	ItemManager::GetIns().SetCamPos(camera.GetPos());
	for (auto& btn : button) {
		if (btn.dummy) {
			btn.dummy->Draw();
			VECTOR camPos = camera.GetPos();
			float distance = VSize(VSub(camPos, btn.pos));
			if (distance <= Scene::Tutorial::BUTTON_UI_VISIBLE_DIST) {
				VECTOR textPos3D = VAdd(btn.pos, VGet(0.0f, Scene::Tutorial::BUTTON_UI_OFFSET_Y, 0.0f));
				VECTOR sp = ConvWorldPosToScreenPos(textPos3D);
				if (sp.z >= 0.0f && sp.z <= 1.0f) {
					MV1_COLL_RESULT_POLY hit = MV1CollCheck_Line(stageHandle, -1, camPos, textPos3D);
					if (hit.HitFlag == 0) {
						const char* name = "";
						if (btn.enemyType == 0) name = "Melee";
						else if (btn.enemyType == 1) name = "Rifle";
						else if (btn.enemyType == 2) name = "Sniper";
						else if (btn.enemyType == 3) name = "Rolling";
						float fadeStartDist = Scene::Tutorial::BUTTON_UI_FADE_START;
						float alphaRate = 1.0f;
						if (distance > fadeStartDist) {
							alphaRate = 1.0f - ((distance - fadeStartDist) / Scene::Tutorial::BUTTON_UI_FADE_RANGE);
						}
						int alpha = static_cast<int>(255 * alphaRate);

						::SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
						::DrawFormatStringToHandle(static_cast<int>(sp.x) + Scene::Tutorial::BUTTON_UI_TEXT_OFFSET_X, static_cast<int>(sp.y), GetColor(Global::Palette::WHITE.r, Global::Palette::WHITE.g, Global::Palette::WHITE.b), fontMedium, "Spawn: %s", name);
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
	DrawBox(0, System::Window::CENTER_Y - Scene::Tutorial::DIALOG_Y_POS, Scene::Tutorial::DIALOG_X2_POS, System::Window::CENTER_Y + Scene::Tutorial::DIALOG_Y_POS, GetColor(Global::Palette::BLACK.r, Global::Palette::BLACK.g, Global::Palette::BLACK.b), true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	const int colorTitle = GetColor(Global::Palette::GREEN.r, Global::Palette::GREEN.g, Global::Palette::GREEN.b);
	const int colorText = GetColor(Global::Palette::WHITE.r, Global::Palette::WHITE.g, Global::Palette::WHITE.b);
	const int colorWarning = GetColor(Global::Palette::RED_LIGHT.r, Global::Palette::RED_LIGHT.g, Global::Palette::RED_LIGHT.b);
	const int colorAlert = GetColor(Global::Palette::YELLOW.r, Global::Palette::YELLOW.g, Global::Palette::YELLOW.b);
	const int colorInfo = GetColor(Global::Palette::AMBER.r, Global::Palette::AMBER.g, Global::Palette::AMBER.b);

	const int TEXT_X = 20;
	const int TEXT_Y = 300;
	const int LINE = 30;


	std::string moveUpKey = TextManager::GetIns().GetActionKeyString(ActionID::MOVE_FORWARD);
	std::string moveDownKey = TextManager::GetIns().GetActionKeyString(ActionID::MOVE_BACK);
	std::string moveLeftKey = TextManager::GetIns().GetActionKeyString(ActionID::MOVE_LEFT);
	std::string moveRightKey = TextManager::GetIns().GetActionKeyString(ActionID::MOVE_RIGHT);
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
		DrawStringToHandle(TEXT_X, TEXT_Y + LINE * 2, ("移動 : " + moveUpKey + " / " + moveDownKey + " / " + moveLeftKey + " / " + moveRightKey).c_str(), colorText, fontSmall);
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
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, Scene::Common::PAUSE_BG_ALPHA);
	DrawBox(0, 0, System::Window::WINDOW_WIDTH, System::Window::WINDOW_HEIGHT, GetColor(Global::Palette::BLACK.r, Global::Palette::BLACK.g, Global::Palette::BLACK.b), TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	int yellow = GetColor(Global::Palette::YELLOW.r, Global::Palette::YELLOW.g, Global::Palette::YELLOW.b);
	int white = GetColor(Global::Palette::WHITE.r, Global::Palette::WHITE.g, Global::Palette::WHITE.b);

	DrawStringToHandle(System::Window::CENTER_X + Scene::Common::PAUSE_TITLE_OFFSET_X, System::Window::CENTER_Y + Scene::Common::PAUSE_TITLE_OFFSET_Y, "PAUSE", white, fontLarge);
	int colorResume = (pauseSelectNum == RESUME) ? yellow : white;
	int colorTitle = (pauseSelectNum == RETURN_TITLE) ? yellow : white;
	DrawStringToHandle(System::Window::CENTER_X + Scene::Common::PAUSE_CURSOR_OFFSET_X, System::Window::CENTER_Y + Scene::Common::PAUSE_ITEM_LINE_HEIGHT * pauseSelectNum, ">", yellow, fontMedium);
	DrawStringToHandle(System::Window::CENTER_X + Scene::Common::PAUSE_ITEM_OFFSET_X, System::Window::CENTER_Y, "Resume", colorResume, fontMedium);
	DrawStringToHandle(System::Window::CENTER_X + Scene::Common::PAUSE_ITEM_OFFSET_X, System::Window::CENTER_Y + Scene::Common::PAUSE_ITEM_LINE_HEIGHT, "Return Title", colorTitle, fontMedium);
}