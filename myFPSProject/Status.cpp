#include "Status.h"
#include "Utils.h"
#include "Debug.h"

#include <fstream>

namespace CHARA_STATUS {
	CharacterStatus PLAYER = { 100, 0.015f, 0.9f, 5, 1.0f, 2.0f, 1.0f,1.8f,0.8f,TEAMID::ID_FRIENDLY, 0};
	CharacterStatus DUMMY = { 10000,0.0f,0.0f, 10, 1.0f,2.0f,2.0f,1.8f,1.8f, TEAMID::ID_ENEMY, 0 };
	CharacterStatus MELEE_ENEMY = { 100,0.023f,0.8f, 1, 1.0f,2.0f,2.0f,1.8f,1.8f,TEAMID::ID_ENEMY , 100};
	CharacterStatus RIFLE_ENEMY = { 120,0.012f,0.8f, 1, 1.0f,2.0f,2.0f,1.8f,1.8f,TEAMID::ID_ENEMY , 150};
	CharacterStatus SNIPER_ENEMY = { 80,0.01f,0.8f, 1, 0.5f,1.5f,1.5f,1.3f,1.3f,TEAMID::ID_ENEMY , 150};
	CharacterStatus ROLL_ENEMY = { 60, 0.01f, 0.95f, 1, 1.0f,1.0f, 1.0f, 0.5f, 0.5f, TEAMID::ID_ENEMY, 50};
}

namespace PLAYER_GUN {
    GunStatus RIFLE = { WeaponID::AR , true, false,true, false, 25, 50.0f, 20, 120, 2.0f, 12.0f, 0.15f, 0.005f, 2.0f, VGet(0.5f,-0.25f,2.34f),VGet(0.0f,0.0f,1.0f), 1.0f,0.4f,50.0f ,0,0,0,0,false,
						{"Resource/Weapon/AR.mv1",0.005f,VGet(0.5f,-0.55f,0.4f),VGet(0.0f,-0.43f,0.4f), VGet(0.5f,-0.25f,2.34f),VGet(0.0f,-0.2f,2.34f),
						 "Resource/WeaponUI/AR.png","Resource/Sound/FireAR.ogg","Resource/Sound/Reload.wav","Resource/Sound/Reloadend.wav"}};

	GunStatus SNIPER = { WeaponID::SR, true, false,false, false, 80, 80.0f, 3, 12, 3.0f, 0.6f, 0.2f, 0.0001f, 7.0f,VGet(0.5f,-0.4f,3.3f), VGet(0.0f,0.0f,1.0f),0.9f, 0.2f, 40.0f,0,0,0,0,false,
						{"Resource/Weapon/SR.mv1",0.005f,VGet(0.5f,-0.55f,0.6f),VGet(0.0f,-0.28f,0.4f), VGet(0.5f,-0.4f,3.3f),VGet(0.0f, -1.2f, 3.3f),
                         "Resource/WeaponUI/SR.png","Resource/Sound/FireSR.ogg","Resource/Sound/Reload.wav","Resource/Sound/Reloadend.wav"} };

	GunStatus LAUNCHER = { WeaponID::LR, false, true, false, false, 180, 40.0f, 1, 5, 2.5f, 10.0f, 0.001f,0.001f, 14.0f,VGet(0.4f,-0.2f,0.94f),VGet(0.4f,-0.2f,0.89f),0.5f,0.5f,65.0f,0.1f,0.5f,10, 0.3f,true,
						{"Resource/Weapon/LR.mv1",0.005f,VGet(0.4f,-0.32f,0.5f),VGet(0.4f,-0.32f,0.5f), VGet(0.4f,-0.2f,0.94f),VGet(0.4f,-0.2f,0.89f),
                         "Resource/WeaponUI/LR.png","Resource/Sound/FireLR.ogg","Resource/Sound/Reload.wav","Resource/Sound/Reloadend.wav"} };

	GunStatus SMG = { WeaponID::SMG, false, false, true, false, 15, 40.0f, 40, 240, 1.8f, 20.0f, 0.1f, 0.03f, 1.5f, VGet(0.4f,-0.2f,1.55f),VGet(0.0f,0.0f,1.0f), 1.0f, 0.7f, 60.0f,0.06f,1.0f,0,0,false,
						{"Resource/Weapon/SMG.mv1",0.005f,VGet(0.5f,-0.6f,0.5f),VGet(0.0f,-0.49f,0.7f), VGet(0.4f,-0.2f, 1.55f),VGet(0.0f,-0.12f,1.55f),
                         "Resource/WeaponUI/SMG.png","Resource/Sound/FireSMG.ogg","Resource/Sound/Reload.wav","Resource/Sound/Reloadend.wav"} };

	GunStatus PISTOL = { WeaponID::PIS, false, false, false, true, 35, 40.0f, 8, 0, 2.0f, 8.0f, 0.05f, 0.005f, 3.0f,VGet(0.5f,-0.25f,1.3f),VGet(0.0f,0.0f,1.0f),1.0f,0.8f,60.0f,0.06f,1.0f,0,0,false,
						{"Resource/Weapon/PIS.mv1",0.005f,VGet(0.5f,-0.55f,0.6f),VGet(0.0f,-0.36f,0.4f), VGet(0.5f,-0.25f,1.3f),VGet(0.0f ,-0.1f ,2.34f),
                         "Resource/WeaponUI/PIS.png","Resource/Sound/FirePIS.ogg","Resource/Sound/Reload.wav","Resource/Sound/Reloadend.wav"} };
}
namespace ENEMY_GUN {
	GunStatus MELEE = { WeaponID::ENEMY_KNIFE,false,false,false,true, 25, 3.0f, 0, 0, 0.0f, 1.0f, 0.0f,0.0f,0.0f,
                        VGet(0.0f,0.0f,0.0f),VGet(0.0f,0.0f,0.0f),1.0f,1.0f,0.0f,0.0f,0.0f,3,0.0f,false ,
						{"Resource/Weapon/EnemyKnife.mv1", 0.008f, VGet(0.4f, 0.0f, 0.5f), VGet(0.0f,0.0f,0.0f), VGet(0.0f,0.0f,0.0f), VGet(0.0f,0.0f,0.0f)} };

	GunStatus RIFLE = {WeaponID::ENEMY_AR, false, false, true, true, 10, 20.0f, 20, 0, 5.0f, 1.0f, 0.001f, 0.001f, 0.0f, 
                        VGet(0.0f,-0.4f,0.0f),VGet(0.0f,0.0f,0.0f), 1.0f,1.0f,50.0f,0.1f,0.3f,0,0,false ,
					    {"Resource/Weapon/EnemyAR.mv1", 0.007f, VGet(0.0f, -0.2f, 0.6f), VGet(0.0f,0.0f,0.0f), VGet(0.0f,0.0f,0.0f), VGet(0.0f,0.0f,0.0f)
                       ,"", "Resource/Sound/FireAR.ogg","Resource/Sound/Reload.wav","Resource/Sound/Reloadend.wav"}};

	GunStatus SNIPER = {WeaponID::ENEMY_SR, false, false, true, true,30, 30.0f, 3, 0, 10.0f, 0.2f, 0.0001f, 0.0001f, 0.0f, 
                         VGet(0.0f,-0.4f,0.0f),VGet(0.0f,0.0f,0.0f), 1.0f, 0.5f, 40.0f, 0.06f, 0.8f,0,0,false,
                        {"Resource/Weapon/EnemySniper.mv1", 0.007f, VGet(0.0f, -0.2f, 0.7f), VGet(0.0f,0.0f,0.0f), VGet(0.0f,0.0f,0.0f), VGet(0.0f,0.0f,0.0f),
                          "","Resource/Sound/FireENEMYSR.ogg","Resource/Sound/Reload.wav","Resource/Sound/Reloadend.wav"}};

	GunStatus DESTRUCT = {WeaponID::ENEMY_EXPLOSION, false, false, false, true,90, 0.0f, 0, 0, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 
                          VGet(0.0f, 0.0f, 0.0f),VGet(0.0f,0.0f,0.0f), 1.0f, 1.0f, 0.0f, 0.0f,0.0f,10,0.5f, true,{} };
}

GameConfigData GameConfig;

void LoadToChara(CharacterStatus& target, const std::vector<std::string>& row) {
    if (row.size() < 12) return;
    try {
        target.maxHP = std::stoi(row[1]);
        target.accel = std::stof(row[2]);
        target.friction = std::stof(row[3]);
        target.mass = std::stoi(row[4]);
        target.width = std::stof(row[5]);
        target.height = std::stof(row[6]);
        target.crouchHeight = std::stof(row[7]);
        target.eyeHeight = std::stof(row[8]);
        target.crouchEyeHeight = std::stof(row[9]);
        target.teamID = (row[10] == "ID_FRIENDLY") ? TEAMID::ID_FRIENDLY : TEAMID::ID_ENEMY;
        target.score = std::stoi(row[11]);
    }
    catch (...) { Debug::Log("Chara.csv Parse Error: " + row[0]); }
}

void LoadToGun(GunStatus& target, const std::vector<std::string>& row) {
    if (row.size() < 47) return;
    try {
        if (row[1] == "AR") target.id = WeaponID::AR;
        else if (row[1] == "SR") target.id = WeaponID::SR;
        else if (row[1] == "LR") target.id = WeaponID::LR;
        else if (row[1] == "SMG") target.id = WeaponID::SMG;
        else if (row[1] == "PIS") target.id = WeaponID::PIS;
        else if (row[1] == "ENEMY_KNIFE") target.id = WeaponID::ENEMY_KNIFE;
        else if (row[1] == "ENEMY_AR") target.id = WeaponID::ENEMY_AR;
        else if (row[1] == "ENEMY_SR") target.id = WeaponID::ENEMY_SR;
        else if (row[1] == "ENEMY_EXPLOSION") target.id = WeaponID::ENEMY_EXPLOSION;

        target.hitscan = (row[2] == "TRUE");
        target.AOE = (row[3] == "TRUE");
        target.fullAuto = (row[4] == "TRUE");
        target.isInfinite = (row[5] == "TRUE");
        target.damage = std::stoi(row[6]);
        target.range = std::stof(row[7]);
        target.magAmmo = std::stoi(row[8]);
        target.bagAmmo = std::stoi(row[9]);
        target.reloadTime = std::stof(row[10]);
        target.fireRate = std::stof(row[11]);
        target.spread = std::stof(row[12]);
        target.adsSpread = std::stof(row[13]);
        target.recoil = std::stof(row[14]);

        target.muzzleOffset = VGet(std::stof(row[15]), std::stof(row[16]), std::stof(row[17]));
        target.adsMuzzleOffset = VGet(std::stof(row[18]), std::stof(row[19]), std::stof(row[20]));

        target.hasDampingRatio = std::stof(row[21]);
        target.adsDampingRatio = std::stof(row[22]);
        target.adsFov = std::stof(row[23]);
        target.projectileSize = std::stof(row[24]);
        target.projectileSpeed = std::stof(row[25]);
        target.explodeArea = std::stoi(row[26]);
        target.knockbackP = std::stof(row[27]);
        target.friendlyFire = (row[28] == "TRUE");

        target.visual.modelPath = row[29];
        target.visual.scale = std::stof(row[30]);
        target.visual.drawOffset = VGet(std::stof(row[31]), std::stof(row[32]), std::stof(row[33]));
        target.visual.adsDrawOffset = VGet(std::stof(row[34]), std::stof(row[35]), std::stof(row[36]));
        target.visual.drawMuzzleOffset = VGet(std::stof(row[37]), std::stof(row[38]), std::stof(row[39]));
        target.visual.drawAdsMuzzleOffset = VGet(std::stof(row[40]), std::stof(row[41]), std::stof(row[42]));

        target.visual.uiPath = row[43];
        target.visual.fireSoundPath = row[44];
        target.visual.reloadSoundPath = row[45];
        target.visual.reloadEndSoundPath = row[46];
    }
    catch (...) { Debug::Log("Gun.csv Parse Error: " + row[0]); }
}

void LoadAllStatusFromCSV() {
    std::string line;

    std::ifstream charaFile("Data/Character.csv");
    if (charaFile) {
        std::getline(charaFile, line);
        while (std::getline(charaFile, line)) {
            auto row = Utils::Split(line, ',');
            if (row.empty()) continue;
            std::string id = row[0];

            if (id == "PLAYER") LoadToChara(CHARA_STATUS::PLAYER, row);
            else if (id == "DUMMY") LoadToChara(CHARA_STATUS::DUMMY, row);
            else if (id == "MELEE") LoadToChara(CHARA_STATUS::MELEE_ENEMY, row);
            else if (id == "RIFLE") LoadToChara(CHARA_STATUS::RIFLE_ENEMY, row);
            else if (id == "SNIPER") LoadToChara(CHARA_STATUS::SNIPER_ENEMY, row);
            else if (id == "ROLLING") LoadToChara(CHARA_STATUS::ROLL_ENEMY, row);
        }
        Debug::Log("Loaded Character.csv");
    }
    else {
        Debug::Log("Failed to open Character.csv");
    }

    std::ifstream gunFile("Data/Gun.csv");
    if (gunFile) {
        std::getline(gunFile, line);
        while (std::getline(gunFile, line)) {
            auto row = Utils::Split(line, ',');
            if (row.empty()) continue;
            std::string id = row[0];

            if (id == "RIFLE") LoadToGun(PLAYER_GUN::RIFLE, row);
            else if (id == "SNIPER") LoadToGun(PLAYER_GUN::SNIPER, row);
            else if (id == "LAUNCHER") LoadToGun(PLAYER_GUN::LAUNCHER, row);
            else if (id == "SMG") LoadToGun(PLAYER_GUN::SMG, row);
            else if (id == "PISTOL") LoadToGun(PLAYER_GUN::PISTOL, row);
            else if (id == "ENEMY_KNIFE") LoadToGun(ENEMY_GUN::MELEE, row);
            else if (id == "ENEMY_AR") LoadToGun(ENEMY_GUN::RIFLE, row);
            else if (id == "ENEMY_SR") LoadToGun(ENEMY_GUN::SNIPER, row);
            else if (id == "DESTRUCT") LoadToGun(ENEMY_GUN::DESTRUCT, row);
        }
        Debug::Log("Loaded Gun.csv");
    }
    else {
        Debug::Log("Failed to open Gun.csv");
    }

    std::ifstream configFile("Data/GameConfig.csv");
    if (configFile) {
        std::getline(configFile, line);
        while (std::getline(configFile, line)) {
            auto row = Utils::Split(line, ',');
            if (row.size() < 2) continue;
            std::string key = row[0];

            try {
                if (key == "MAXENEMY_ONMAP") GameConfig.maxEnemyOnMap = std::stoi(row[1]);
                else if (key == "MAX_LIMIT_MELEE") GameConfig.maxLimitMelee = std::stoi(row[1]);
                else if (key == "MAX_LIMIT_RIFLE") GameConfig.maxLimitRifle = std::stoi(row[1]);
                else if (key == "MAX_LIMIT_SNIPER") GameConfig.maxLimitSniper = std::stoi(row[1]);
                else if (key == "MAX_LIMIT_ROLLING") GameConfig.maxLimitRolling = std::stoi(row[1]);
                else if (key == "INIT_SPAWN_INTERVAL") GameConfig.initSpawnInterval = std::stof(row[1]);
                else if (key == "PREPARE_TIME") GameConfig.prepareTime = std::stof(row[1]);
                else if (key == "DIFFICULTY_UP_INTERVAL") GameConfig.difficultyUpInterval = std::stof(row[1]);
                else if (key == "DIFFICULTY_UP_AMOUNT") GameConfig.difficultyUpAmount = std::stof(row[1]);
                else if (key == "MIN_SPAWN_INTERVAL") GameConfig.minSpawnInterval = std::stof(row[1]);
            }
            catch (...) { Debug::Log("GameConfig.csv Parse Error: " + key); }
        }
        Debug::Log("Loaded GameConfig.csv");
    }
    else {
        Debug::Log("Failed to open GameConfig.csv");
    }
}