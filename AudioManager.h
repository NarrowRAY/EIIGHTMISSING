#pragma once
#include <SFML/Audio.hpp>
#include <string>
#include <unordered_map>
#include <random>

// ============================================================
// 音频管理器 — 管理所有背景音乐和音效
// ============================================================
class AudioManager {
public:
    AudioManager() = default;
    ~AudioManager() = default;

    // 初始化：加载所有音效
    bool Init();

    // ---- 背景音乐 ----
    void PlayBGM(const std::string& name, bool loop = true);
    void StopBGM();
    void PauseBGM();
    void ResumeBGM();
    void SetBGMVolume(float vol);   // 0.0 ~ 100.0
    void SetSFXVolume(float vol);   // 0.0 ~ 100.0

    // ---- 音效 ----
    void PlaySFX(const std::string& name);
    void PlayRandomSFX(const std::string& base, int count);  // 如 footstep_1/2/3

private:
    bool LoadSFX(const std::string& name, const std::string& path);

    sf::Music m_bgm;
    std::unordered_map<std::string, sf::SoundBuffer> m_buffers;
    std::vector<sf::Sound> m_activeSounds;  // 同时播放多个音效
    std::mt19937 m_rng{std::random_device{}()};

    float m_bgmVolume = 50.f;
    float m_sfxVolume = 80.f;

    // 定期清理已停止的音效
    void CleanSounds();
    static constexpr int MAX_SOUNDS = 16;
};
