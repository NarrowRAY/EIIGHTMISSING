#include "Game.h"
#include "SavePanel.h"
#include "Scene_Title.h"
#include "Scene_1B.h"
#include "Scene_Stairwell1.h"
#include "Scene_Stairwell2.h"
#include "Scene_2_1.h"
#include "Scene_2_2.h"
#include "Scene_2_3.h"
#include "Scene_Dream.h"
#include <windows.h>
#include <imm.h>
#pragma comment(lib, "imm32.lib")

Game::Game() {
    InitWindow();
    InitFont();
    InitAudio();
    m_sceneManager.Push(std::make_unique<TitleScene>(&m_font));
}

void Game::InitWindow() {
    m_window.create(sf::VideoMode({1280, 720}), U("八点迷思"), sf::State::Windowed);
    m_window.setFramerateLimit(60);
    m_window.setVerticalSyncEnabled(true);
    // 窗口图标
    sf::Image icon;
    if (icon.loadFromFile("assets/textures/icon.png"))
        m_window.setIcon(icon);
    // 禁用输入法，防止 WASD 触发中文输入
    HWND hwnd = m_window.getNativeHandle();
    ImmAssociateContext(hwnd, nullptr);
}

void Game::InitAudio() {
    m_audio.Init();
}

void Game::InitFont() {
    const char* fontPaths[] = {
        "C:\\Windows\\Fonts\\msyh.ttc",
        "C:\\Windows\\Fonts\\simhei.ttf",
        "C:\\Windows\\Fonts\\simsun.ttc",
        nullptr
    };
    for (int i = 0; fontPaths[i]; ++i) {
        if (m_font.openFromFile(fontPaths[i])) return;
    }
    (void)m_font.openFromFile("C:\\Windows\\Fonts\\arial.ttf");
}

void Game::Run() {
    sf::View defaultView({SCREEN_W / 2.f, SCREEN_H / 2.f}, {SCREEN_W, SCREEN_H});

    while (m_window.isOpen()) {
        float dt = m_clock.restart().asSeconds();
        if (dt > 0.1f) dt = 0.1f;

        HandleEvents();

        m_window.setView(defaultView);
        Scene* cur = m_sceneManager.Current();
        if (cur) {
            cur->HandleInput(m_window);
            cur->Update(dt);
        }

        HandleSceneChange();
        UpdateBGM();


        m_window.setView(defaultView);
        m_window.clear(Colors::BG);
        UpdateView();
        cur = m_sceneManager.Current();
        if (cur) cur->Draw(m_window);
        m_window.display();
    }
}

void Game::HandleEvents() {
    while (auto event = m_window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            m_window.close();
        }
        else if (const auto* kp = event->getIf<sf::Event::KeyPressed>()) {
            if (kp->scancode == sf::Keyboard::Scancode::Escape) {
                m_window.close();
            }
            else {
                Scene* cur = m_sceneManager.Current();
                if (cur) cur->HandleEvent(*event);
            }
        }
    }
}

void Game::HandleSceneChange() {
    Scene* cur = m_sceneManager.Current();
    if (!cur) return;
    if (cur->ShouldPop()) { m_sceneManager.Pop(); return; }

    int nextId = cur->GetNextScene();
    int entryParam = cur->GetEntryParam();  // 必须在 Reset 之前读
    if (nextId == -2) { m_window.close(); return; }
    if (nextId < 0) return;

    cur->Reset();
    SceneID next = static_cast<SceneID>(nextId);
    m_sceneManager.Replace(CreateScene(next, entryParam));
}

void Game::UpdateBGM() {
    Scene* cur = m_sceneManager.Current();
    if (!cur) return;

    int targetBGM = cur->GetBGMId();
    if (targetBGM == m_currentBGMScene) return;
    m_currentBGMScene = targetBGM;

    switch (targetBGM) {
        case 0: m_audio.PlayBGM("bgm_gentle", true); break;   // 游戏场景
        case 1: m_audio.StopBGM(); break;                      // 标题画面（无声或单独BGM）
        default: break;
    }
}

void Game::UpdateView() {
    sf::Vector2u winSize = m_window.getSize();
    float winW = static_cast<float>(winSize.x);
    float winH = static_cast<float>(winSize.y);
    float ratio = winW / winH;
    float target = static_cast<float>(SCREEN_W) / SCREEN_H;

    sf::View view;
    view.setCenter({SCREEN_W / 2.f, SCREEN_H / 2.f});
    view.setSize({SCREEN_W, SCREEN_H});

    if (ratio > target) {
        float h = SCREEN_W / ratio;
        view.setViewport({{0.f, (1.f - h / SCREEN_H) / 2.f}, {1.f, h / SCREEN_H}});
    } else {
        float w = SCREEN_H * ratio;
        view.setViewport({{(1.f - w / SCREEN_W) / 2.f, 0.f}, {w / SCREEN_W, 1.f}});
    }
    m_window.setView(view);
}

std::unique_ptr<Scene> Game::CreateScene(SceneID id, int entryParam) {
    std::unique_ptr<Scene> scene;
    switch (id) {
        case SceneID::Title:       scene = std::make_unique<TitleScene>(&m_font); break;
        case SceneID::Classroom1B: scene = std::make_unique<Scene_1B>(entryParam); break;
        case SceneID::Stairwell1:  scene = std::make_unique<Scene_Stairwell1>(entryParam); break;
        case SceneID::Stairwell2:  scene = std::make_unique<Scene_Stairwell2>(entryParam); break;
        case SceneID::Corridor2_1: scene = std::make_unique<Scene_2_1>(entryParam); break;
        case SceneID::Corridor2_2: scene = std::make_unique<Scene_2_2>(entryParam); break;
        case SceneID::Corridor2_3: scene = std::make_unique<Scene_2_3>(entryParam); break;
        case SceneID::Dream:       scene = std::make_unique<Scene_Dream>(entryParam); break;
        default: return nullptr;
    }
    if (auto* gs = dynamic_cast<GameScene*>(scene.get())) {
        gs->SetFont(&m_font);
        gs->SetAudio(&m_audio);
        gs->SetSceneID(static_cast<int>(id));
        // 读档：直接设置玩家位置（LoadMap 已在构造函数中执行，需覆盖）
        if (SavePanel::HasPendingLoad()) {
            gs->SetPlayerPosition({SavePanel::GetPendingX(), SavePanel::GetPendingY()});
            GameScene::s_passwordUnlocked = SavePanel::GetPendingPwd();
            SavePanel::ClearPending();
        }
    }
    return scene;
}
