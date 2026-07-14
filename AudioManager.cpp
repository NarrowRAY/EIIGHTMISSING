#include "AudioManager.h"
#include <iostream>

bool AudioManager::Init() {
    // ---- 音效 ----
    const char* sfxDir = "assets/audio/sfx/";

    // UI 音效 (来自 Kenney，CC0)
    LoadSFX("click",   std::string(sfxDir) + "click.wav");
    LoadSFX("hover",   std::string(sfxDir) + "hover.wav");
    LoadSFX("beep",    std::string(sfxDir) + "beep.wav");
    LoadSFX("interact",std::string(sfxDir) + "interact.wav");

    // 环境音效 (程序生成)
    LoadSFX("footstep_1", std::string(sfxDir) + "footstep_1.wav");
    LoadSFX("footstep_2", std::string(sfxDir) + "footstep_2.wav");
    LoadSFX("footstep_3", std::string(sfxDir) + "footstep_3.wav");
    LoadSFX("door_open",  std::string(sfxDir) + "door_open.wav");
    LoadSFX("door_close", std::string(sfxDir) + "door_close.wav");

    return true;
}

bool AudioManager::LoadSFX(const std::string& name, const std::string& path) {
    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile(path)) {
        // 加载失败不阻塞游戏，仅输出警告
        std::cerr << "[Audio] Failed to load: " << path << std::endl;
        return false;
    }
    m_buffers[name] = std::move(buffer);
    return true;
}

// ---- 背景音乐 ----
void AudioManager::PlayBGM(const std::string& name, bool loop) {
    m_bgm.stop();

    std::string path = "assets/audio/music/" + name + ".wav";
    if (!m_bgm.openFromFile(path)) {
        std::cerr << "[Audio] Failed to open BGM: " << path << std::endl;
        return;
    }

    m_bgm.setLooping(loop);
    m_bgm.setVolume(m_bgmVolume);
    m_bgm.play();
}

void AudioManager::StopBGM() {
    m_bgm.stop();
}

void AudioManager::PauseBGM() {
    m_bgm.pause();
}

void AudioManager::ResumeBGM() {
    if (m_bgm.getStatus() == sf::SoundSource::Status::Paused)
        m_bgm.play();
}

void AudioManager::SetBGMVolume(float vol) {
    m_bgmVolume = vol;
    m_bgm.setVolume(vol);
}

void AudioManager::SetSFXVolume(float vol) {
    m_sfxVolume = vol;
}

// ---- 音效 ----
void AudioManager::PlaySFX(const std::string& name) {
    auto it = m_buffers.find(name);
    if (it == m_buffers.end()) return;

    CleanSounds();

    // 用过的旧 Sound 对象复用（已停止的）
    for (auto& snd : m_activeSounds) {
        if (snd.getStatus() == sf::SoundSource::Status::Stopped) {
            snd.setBuffer(it->second);
            snd.setVolume(m_sfxVolume);
            snd.play();
            return;
        }
    }

    // 创建新的 Sound（限制数量）
    if (m_activeSounds.size() >= MAX_SOUNDS) return;

    sf::Sound snd(it->second);
    snd.setVolume(m_sfxVolume);
    snd.play();
    m_activeSounds.push_back(std::move(snd));
}

void AudioManager::PlayRandomSFX(const std::string& base, int count) {
    std::uniform_int_distribution<int> dist(1, count);
    int idx = dist(m_rng);
    PlaySFX(base + "_" + std::to_string(idx));
}

void AudioManager::CleanSounds() {
    m_activeSounds.erase(
        std::remove_if(m_activeSounds.begin(), m_activeSounds.end(),
            [](sf::Sound& s) {
                return s.getStatus() == sf::SoundSource::Status::Stopped;
            }),
        m_activeSounds.end()
    );
}
