#include"Debug.h"
#include<cstdio>
#include<iomanip>
bool Debug::s_enabled = true;
std::vector<std::string> Debug::s_logList;
std::vector < std::pair < std::string, std::string >> Debug::s_watchList;

int Debug::s_logPosX = 20;
int Debug::s_logPosY = 20;
int Debug::s_watchPosX = 400;
int Debug::s_watchPosY = 20;


void Debug::Update() {

	const int move = 4;
	
	if (CheckHitKey(KEY_INPUT_I)) {
		if (CheckHitKey(KEY_INPUT_UP))s_watchPosY -= move;
		if (CheckHitKey(KEY_INPUT_LEFT))s_watchPosX -= move;
		if (CheckHitKey(KEY_INPUT_DOWN))s_watchPosY += move;
		if (CheckHitKey(KEY_INPUT_RIGHT))s_watchPosX += move;
	}
	if (CheckHitKey(KEY_INPUT_L)) {
		if (CheckHitKey(KEY_INPUT_UP))s_logPosY -= move;
		if (CheckHitKey(KEY_INPUT_LEFT))s_logPosX -= move;
		if (CheckHitKey(KEY_INPUT_DOWN))s_logPosY += move;
		if (CheckHitKey(KEY_INPUT_RIGHT))s_logPosX += move;
	}
}
void Debug::SetEnabled(bool tag) {
	s_enabled = tag;
}

bool Debug::IsEnabled() {
	return s_enabled;
}


void Debug::Log(const std::string& text) {
	if (!s_enabled) return;
	s_logList.push_back(text);
	if (s_logList.size() > 10) s_logList.erase(s_logList.begin());	//10Œ‚Ü‚Å•ÛŽ
}

void Debug::SetWatchString(const std::string& name, const std::string& value) {
	for (auto& p : s_watchList) {
		if (p.first == name) {
			p.second = value;
			return;
		}
	}
	s_watchList.push_back({ name,value });
}

void Debug::Watch(const std::string& name, const std::string& value){
	if (!s_enabled)return;
	SetWatchString(name, value);
}
void Debug::Watch(const std::string& name, const char* value) {
	if (!s_enabled)return;
	SetWatchString(name, std::string(value));
}

void Debug::Draw() {
	if (!s_enabled)return;

	int logHeight = s_logList.size() * 16 + 10;
	int watchHeight = s_watchList.size() * 16 + 10;

	int background = GetColor(0, 0, 0);
	int border = GetColor(255, 0, 255);
	int posY = s_logPosY;

	// ƒQ[ƒ€‰æ–Êã‚ÉƒƒO‚ð•`‰æ
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 80);
	DrawBox(s_logPosX - 5, s_logPosY - 5, s_logPosX + BOX_WIDTH, s_logPosY + logHeight, background, TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	if (CheckHitKey(KEY_INPUT_L)) {
		DrawBox(s_logPosX - 5, s_logPosY - 5, s_logPosX + BOX_WIDTH, s_logPosY + logHeight, border, FALSE);
	}

	for (int i=(int)s_logList.size()-1;i>=0;--i) {
		DrawString(s_logPosX, posY, s_logList[i].c_str(), GetColor(255, 255, 0));
		posY += 16;
	}


	posY = s_watchPosY;

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, BGALPHA);
	DrawBox(s_watchPosX - 5, s_watchPosY - 5, s_watchPosX + 250, s_watchPosY + watchHeight, background, TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	if (CheckHitKey(KEY_INPUT_I)) {
		DrawBox(s_watchPosX - 5, s_watchPosY - 5, s_watchPosX + 250, s_watchPosY + watchHeight, border, FALSE);
	}

	for (const auto& pair : s_watchList) {
		std::string text = pair.first + ": " + pair.second;
		DrawString(s_watchPosX, posY, text.c_str(), GetColor(0, 255, 0));
		posY += 16;
	}
}

void Debug::Clear() {
	s_logList.clear();
	s_watchList.clear();
}