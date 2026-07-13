#pragma once
#include <SFML/Graphics.hpp>
#include "Scene.h"
#include "Constants.h"
#include "AudioManager.h"

class Game {
public:
    Game();
    ~Game() = default;
    void Run();

private:
    void InitWindow();
    void InitFont();
    void InitAudio();
    void HandleEvents();
    void HandleSceneChange();
    void UpdateView();
    void UpdateBGM();

    std::unique_ptr<Scene> CreateScene(SceneID id, int entryParam);

    sf::RenderWindow m_window;
    sf::Font         m_font;
    SceneManager     m_sceneManager;
    sf::Clock        m_clock;
    AudioManager     m_audio;
    int              m_currentBGMScene = -1;
};
