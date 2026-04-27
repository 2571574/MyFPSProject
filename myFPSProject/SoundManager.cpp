#include "SoundManager.h"
#include "ResourceManager.h"
#include "ConfigManager.h"
#include "Time.h"
#include "Param/System.h"
#include "Param/Global.h"

SoundManager& SoundManager::GetIns() {
    static SoundManager ins;
    return ins;
}

void SoundManager::Update() {
    float dt = Time::GetIns().GetDelta();

    if (currentBGMHandle != -1 && CheckSoundMem(currentBGMHandle) == 1) {
        float bgmVol = ConfigManager::GetIns().Settings().bgmVolume;
        ChangeVolumeSoundMem(static_cast<int>(System::Sound::MAX_VOLUME_SCALE * bgmVol), currentBGMHandle);
    }

    float seVolBase = ConfigManager::GetIns().Settings().seVolume;
    for (auto it = fadingSEs.begin(); it != fadingSEs.end();) {
        it->timer -= dt;
        if (it->timer <= 0.0f) {
            StopSoundMem(it->handle);
            it = fadingSEs.erase(it);
        }
        else {
            float rate = it->timer / it->maxTime;
            int vol = static_cast<int>(System::Sound::MAX_VOLUME_SCALE * seVolBase * rate);
            ChangeVolumeSoundMem(vol, it->handle);
            ++it;
        }
    }
}

void SoundManager::Clear() {
    StopBGM();
    fadingSEs.clear();
    for (auto& pair : sePools) {
        for (int handle : pair.second.duplicateHandles) {
            DeleteSoundMem(handle);
        }
    }
    sePools.clear();
}

void SoundManager::PlayBGM(const std::string& path) {
    SetCreate3DSoundFlag(FALSE);
    int nextBGM = ResourceManager::GetIns().GetSound(path);
    SetCreate3DSoundFlag(TRUE);
    if (nextBGM == -1 || nextBGM == currentBGMHandle) return;

    StopBGM();
    currentBGMHandle = nextBGM;

    float bgmVol = ConfigManager::GetIns().Settings().bgmVolume;
    ChangeVolumeSoundMem(static_cast<int>(System::Sound::MAX_VOLUME_SCALE * bgmVol), currentBGMHandle);
    PlaySoundMem(currentBGMHandle, DX_PLAYTYPE_LOOP);
}

void SoundManager::StopBGM() {
    if (currentBGMHandle != -1) {
        StopSoundMem(currentBGMHandle);
        currentBGMHandle = -1;
    }
}

void SoundManager::PlaySE(const std::string& path) {
    SetCreate3DSoundFlag(FALSE);
    int baseHandle = ResourceManager::GetIns().GetSound(path);
    SetCreate3DSoundFlag(TRUE);
    if (baseHandle == -1) return;

    auto& pool = sePools[path];
    pool.baseHandle = baseHandle;

    int playHandle = -1;
    for (int handle : pool.duplicateHandles) {
        if (CheckSoundMem(handle) == 0) {
            playHandle = handle;
            break;
        }
    }

    if (playHandle == -1 && pool.duplicateHandles.size() < System::Sound::MAX_DUPLICATE) {
        playHandle = DuplicateSoundMem(baseHandle);
        if (playHandle != -1) pool.duplicateHandles.push_back(playHandle);
    }

    if (playHandle != -1) {
        float seVol = ConfigManager::GetIns().Settings().seVolume;
        ChangeVolumeSoundMem(static_cast<int>(System::Sound::MAX_VOLUME_SCALE * seVol), playHandle);
        PlaySoundMem(playHandle, DX_PLAYTYPE_BACK);
    }
}

void SoundManager::PlaySEWithFadeOut(const std::string& path, float fadeTimeSec) {
    SetCreate3DSoundFlag(FALSE);
    int baseHandle = ResourceManager::GetIns().GetSound(path);
    SetCreate3DSoundFlag(TRUE);
    if (baseHandle == -1) return;

    auto& pool = sePools[path];
    int playHandle = -1;
    for (int handle : pool.duplicateHandles) {
        if (CheckSoundMem(handle) == 0) {
            playHandle = handle; break;
        }
    }
    if (playHandle == -1 && pool.duplicateHandles.size() < System::Sound::MAX_DUPLICATE) {
        playHandle = DuplicateSoundMem(baseHandle);
        if (playHandle != -1) pool.duplicateHandles.push_back(playHandle);
    }

    if (playHandle != -1) {
        float seVol = ConfigManager::GetIns().Settings().seVolume;
        ChangeVolumeSoundMem(static_cast<int>(System::Sound::MAX_VOLUME_SCALE * seVol), playHandle);
        PlaySoundMem(playHandle, DX_PLAYTYPE_BACK);

        fadingSEs.push_back({ playHandle, fadeTimeSec, fadeTimeSec });
    }
}

int SoundManager::Play3DSE(const std::string& path, VECTOR position, float radius) {
    SetCreate3DSoundFlag(TRUE);
    int baseHandle = ResourceManager::GetIns().GetSound(path);
    if (baseHandle == -1) return -1;

    auto& pool = sePools[path];
    int playHandle = -1;
    for (int handle : pool.duplicateHandles) {
        if (CheckSoundMem(handle) == 0) {
            playHandle = handle; break;
        }
    }

    if (playHandle == -1 && pool.duplicateHandles.size() < System::Sound::MAX_DUPLICATE) {
        playHandle = DuplicateSoundMem(baseHandle);
        if (playHandle != -1) pool.duplicateHandles.push_back(playHandle);
    }

    if (playHandle != -1) {
        float seVol = ConfigManager::GetIns().Settings().seVolume;
        ChangeVolumeSoundMem(static_cast<int>(System::Sound::MAX_VOLUME_SCALE * seVol), playHandle);

        Set3DRadiusSoundMem(radius, playHandle);
        Set3DPositionSoundMem(position, playHandle);

        PlaySoundMem(playHandle, DX_PLAYTYPE_BACK);
    }
    return playHandle;
}


void SoundManager::StopSE(const std::string& path) {
    auto it = sePools.find(path);
    if (it != sePools.end()) {
        for (int handle : it->second.duplicateHandles) {
            if (CheckSoundMem(handle) == 1) {
                StopSoundMem(handle);
            }
        }
    }
}

void SoundManager::StopSE(int handle) {
    if (handle != -1 && CheckSoundMem(handle) == 1) {
        StopSoundMem(handle);
    }
}

void SoundManager::PauseAll() {
    pausedSEHandles.clear();

    if (currentBGMHandle != -1 && CheckSoundMem(currentBGMHandle) == 1) {
        StopSoundMem(currentBGMHandle);
        isBGMPaused = true;
    }
    else {
        isBGMPaused = false;
    }

    for (auto& fade : fadingSEs) {
        if (CheckSoundMem(fade.handle) == 1) {
            StopSoundMem(fade.handle);
            pausedSEHandles.push_back(fade.handle);
        }
    }

    for (auto& pair : sePools) {
        for (int handle : pair.second.duplicateHandles) {
            if (CheckSoundMem(handle) == 1) {
                bool alreadyAdded = false;
                for (int pHandle : pausedSEHandles) {
                    if (pHandle == handle) {
                        alreadyAdded = true;
                        break;
                    }
                }
                if (!alreadyAdded) {
                    StopSoundMem(handle);
                    pausedSEHandles.push_back(handle);
                }
            }
        }
    }
}

void SoundManager::ResumeAll() {
    if (isBGMPaused && currentBGMHandle != -1) {
        PlaySoundMem(currentBGMHandle, DX_PLAYTYPE_LOOP, FALSE);
        isBGMPaused = false;
    }

    for (int handle : pausedSEHandles) {
        PlaySoundMem(handle, DX_PLAYTYPE_BACK, FALSE);
    }
    pausedSEHandles.clear();
}


void SoundManager::StopAll() {
    StopBGM();

    for (auto& fade : fadingSEs) {
        if (CheckSoundMem(fade.handle) == 1) {
            StopSoundMem(fade.handle);
        }
    }
    fadingSEs.clear();

    for (auto& pair : sePools) {
        for (int handle : pair.second.duplicateHandles) {
            if (CheckSoundMem(handle) == 1) {
                StopSoundMem(handle);
            }
        }
    }
    pausedSEHandles.clear();
    isBGMPaused = false;
}

void SoundManager::UpdateListener(VECTOR pos, VECTOR front, VECTOR up) {
    Set3DSoundListenerPosAndFrontPosAndUpVec(pos, VAdd(pos, front), up);
}

float SoundManager::GetSoundDuration(const std::string& path) {
    int baseHandle = ResourceManager::GetIns().GetSound(path);
    if (baseHandle == -1) return 0.0f;

    return GetSoundTotalTime(baseHandle) * Global::Math::MS_TO_SEC;
}